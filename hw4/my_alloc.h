#pragma once

#include <memory>

#define DEBUG

using byte = unsigned char;

class MemoryAllocator {
public:
    virtual void init() = 0;
    virtual void destroy() = 0;

    virtual void *alloc(size_t size) = 0;
    virtual void free(void *p) = 0;

#ifdef DEBUG
    virtual void dumpStat() const = 0;
    virtual void dumpBlocks() const = 0;
#endif
};

class FMA : MemoryAllocator {
public:
    FMA(size_t sizeOfBlock, size_t nBlocks) : sizeOfBlock_(sizeOfBlock), nBlocks_(nBlocks),
        nNextForAlloc(0), pool_(nullptr), nUsed_(0) {
        sizeOfBlock_ = std::max(sizeOfBlock_, sizeof(size_t));

#ifdef DEBUG
        nAlloc = 0;
        nFree = 0;
#endif
    }

    ~FMA() {
        destroy();
    }

    void init() override {
        pool_ = new byte[nBlocks_ * sizeOfBlock_];
        for (size_t i = 0; i < nBlocks_ - 1; ++i) {
            size_t* pBlock = (size_t*)getAddresOfBlock(i);
            *pBlock = i + 1;
        }
    }

    void destroy() override {
        delete[] pool_;
    }

    void *alloc(size_t size) override {
        if (size > sizeOfBlock_ || nUsed_ >= nBlocks_) {
            return nullptr;
        }

        byte* retPtr = getAddresOfBlock(nNextForAlloc);
        nNextForAlloc = *(size_t*)retPtr;
        ++nUsed_;
#ifdef DEBUG
        ++nAlloc;
        allocBlocks_.insert(getOrderOfBlockByAddres(retPtr));
#endif
        return retPtr;
    }

    void free(void *p) override {
        *(size_t*)p = nNextForAlloc;
        nNextForAlloc = *(size_t*)p;
        --nUsed_;
#ifdef DEBUG
        ++nFree;
        allocBlocks_.erase(getOrderOfBlockByAddres((byte*)p));
#endif
    }

#ifdef DEBUG
    void dumpStat() const override {
        printf("dump stats after %lu alloc, %lu free\n", nAlloc, nFree);
        for (size_t i = 0; i < nBlocks_; ++i) {
            bool isFree = !allocBlocks_.count(i);
            if (isFree) {
                printf("+++ |--%lu--|\n", sizeOfBlock_);
            }
        }
    }

    void dumpBlocks() const override {
        printf("dump stats after %lu alloc, %lu free\n", nAlloc, nFree);
        for (size_t i = 0; i < nBlocks_; ++i) {
            bool isFree = !allocBlocks_.count(i);
            if (isFree) {
                printf("--- |--%lu--|\n", sizeOfBlock_);
            } else {
                printf("+++ |--%lu--|\n", sizeOfBlock_);
            }
        }
    }
#endif

private:

    byte* getAddresOfBlock(size_t n) {
        assert(n < nBlocks_);
        return pool_ + n * sizeOfBlock_;
    }

    size_t getOrderOfBlockByAddres(byte* addr) {
        assert((size_t)(addr - pool_) % sizeOfBlock_ == 0);
        return (size_t)(addr - pool_) / sizeOfBlock_;
    }

#ifdef DEBUG
    size_t nAlloc;
    size_t nFree;
    std::set<size_t > allocBlocks_;
#endif

    size_t sizeOfBlock_;
    size_t nBlocks_;
    size_t nUsed_;
    size_t nNextForAlloc;
    byte* pool_;
};


class FreeList : MemoryAllocator {
private:
    struct Header {
#ifdef DEBUG
        size_t id;
#endif
        size_t size;
        Header* prev;
        Header* next;
    };
    using Footer = Header*;

    static size_t getOverheadBytes() {
        return sizeof(Header) + sizeof(Footer);
    }

    static void* getUserPointer(const Header* hdr) {
        return (void*)((byte*)hdr + sizeof(Header));
    }

    static Footer* getFooterPointer(const Header* hdr) {
        return (Footer*)((byte*)hdr + sizeof(Header) + hdr->size);
    }

    static Header* FreeList_Remove(Header* hdr) {
        if (hdr->prev == hdr) {
            assert(hdr->next == hdr);
            return nullptr;
        } else {
            hdr->prev->next = hdr->next;
            hdr->next->prev = hdr->prev;
            return hdr->prev;
        }
    }

    static void FreeList_InsertAfter(Header* after, Header* hdr) {
        after->next->prev = hdr;
        hdr->next = after->next;
        after->next = hdr;
        hdr->prev = after;
    }
public:

    explicit FreeList(size_t size) : size_(size) {
        assert(size >= getOverheadBytes());
#ifdef DEBUG
        nAlloc = 0;
        nFree = 0;
#endif
    }

    ~FreeList() {
        destroy();
    }

    void init() override {
        data_ = new byte[size_];

        freeList_ = (Header*)data_;
        freeList_->size = size_ - getOverheadBytes();
        freeList_->prev = freeList_;
        freeList_->next = freeList_;
    }

    void destroy() override {
        delete[] data_;
    }

    void* alloc(size_t size) override {
        if (freeList_) {
            const Header* startHdr = freeList_;
            Header* iterBlockHdr = freeList_;
            do {
                if (iterBlockHdr->size >= size) {
                    freeList_ = FreeList_Remove(iterBlockHdr);

                    if (iterBlockHdr->size > size + getOverheadBytes()) {
                        // [(iter header)                       (iter size)                    (iter footer)] ->
                        // [(iter header) (iter size) (iter footer)] + [(new header) (new size) (new footer)]
                        Header* newBlockHdr = (Header*)((byte*)iterBlockHdr + getOverheadBytes() + size);
                        newBlockHdr->size = iterBlockHdr->size - getOverheadBytes() - size;
                        *getFooterPointer(newBlockHdr) = newBlockHdr;

                        iterBlockHdr->size = size;
                        *getFooterPointer(iterBlockHdr) = nullptr;

                        if (freeList_) {
                            FreeList_InsertAfter(freeList_, newBlockHdr);
                        } else {
                            newBlockHdr->prev = newBlockHdr;
                            newBlockHdr->next = newBlockHdr;
                            freeList_ = newBlockHdr;
                        }
                    }

#ifdef DEBUG
                    iterBlockHdr->id = nAlloc;
                    ++nAlloc;
#endif
                    return getUserPointer(iterBlockHdr);
                }
                iterBlockHdr = iterBlockHdr->next;
            } while (iterBlockHdr == startHdr);
        }

        return nullptr;
    }

    void free(void *p) override {
        Header* removeBlockHdr = (Header*)((byte*)p - sizeof(Header));
        if (freeList_) {
            byte* addrPrevBlockFooter = (byte*)removeBlockHdr - sizeof(Footer);
            Footer* prevBlockFooter = (Footer*)(addrPrevBlockFooter);
            if (addrPrevBlockFooter > data_ && *prevBlockFooter) {
                Header* prevBlockHdr = *prevBlockFooter;
                prevBlockHdr->size += getOverheadBytes() + removeBlockHdr->size;
                freeList_ = prevBlockHdr;
            } else {
                FreeList_InsertAfter(freeList_, removeBlockHdr);
            }

            byte* addrNextBlockHdr = (byte*)getFooterPointer(removeBlockHdr) + sizeof(Footer);
            Header* nextBlockHdr = (Header*)(addrNextBlockHdr);
            if (addrNextBlockHdr < data_ + size_ && *getFooterPointer(nextBlockHdr)) {
                FreeList_Remove(nextBlockHdr);
                removeBlockHdr->size += getOverheadBytes() + nextBlockHdr->size;
                freeList_ = removeBlockHdr;
            }
        } else {
            removeBlockHdr->prev = removeBlockHdr;
            removeBlockHdr->next = removeBlockHdr;
            freeList_ = removeBlockHdr;
        }

#ifdef DEBUG
        ++nFree;
#endif
    }

#ifdef DEBUG
    void dumpStat() const override {
        std::set<Header*> hdrs;
        if (freeList_) {
            const Header* startBlockHdr = freeList_;
            Header* iterBlockHdr = freeList_;
            do {
                hdrs.insert(iterBlockHdr);
                iterBlockHdr = iterBlockHdr->next;
            } while (iterBlockHdr != startBlockHdr);
        }

        printf("dump stats after %lu alloc, %lu free\n", nAlloc, nFree);

        Header* iterBlock = (Header*)data_;
        while ((byte*)iterBlock < data_ + size_) {
            bool isFree = hdrs.count(iterBlock);
            if (isFree) {
                printf("--- |--%lu--|\n", iterBlock->size);
            } else {
                printf("+++ |--%lu--| id: %lu\n", iterBlock->size, iterBlock->id);
            }

            iterBlock = (Header*)((byte*)iterBlock + getOverheadBytes() + iterBlock->size);
        }
    }

    void dumpFreeList(bool doSortByOffset = false) const {
        printf("dump free list after %lu alloc, %lu free\n", nAlloc, nFree);
        if (freeList_) {
            const Header* startBlockHdr = freeList_;

            std::vector<Header*> hdrs;
            Header* iterBlockHdr = freeList_;
            do {
                hdrs.emplace_back(iterBlockHdr);
                iterBlockHdr = iterBlockHdr->next;
            } while (iterBlockHdr != startBlockHdr);

            if (doSortByOffset) {
                std::sort(hdrs.begin(), hdrs.end(), [] (const Header* r, const Header* l) { return r < l; });
            }

            for (const auto &hdr : hdrs) {
                printf(" { offset: %lu, size: %lu }\n", (size_t)((byte*)hdr - data_), hdr->size);
            }
        } else {
            printf("free list is empty\n");
        }
    }

    byte* getDataPtr() {
        return data_;
    }

    void dumpBlocks() const override {
        if (freeList_) {
            const Header* startBlockHdr = freeList_;

            std::vector<Header*> hdrs;
            Header* iterBlockHdr = freeList_;
            do {
                hdrs.emplace_back(iterBlockHdr);
                iterBlockHdr = iterBlockHdr->next;
            } while (iterBlockHdr != startBlockHdr);

            std::sort(hdrs.begin(), hdrs.end(), [] (const Header* r, const Header* l) { return r < l; });

            printf("alloc dump after %lu alloc, %lu free\n", nAlloc, nFree);
            for (const auto &hdr : hdrs) {
                printf("alloc addr: %p, size: %lu\n", hdr, hdr->size);
            }
        }
    }
#endif

private:
#ifdef DEBUG
    size_t nAlloc;
    size_t nFree;
#endif

    size_t size_;
    byte* data_;
    Header* freeList_;
};