// logarithmic.cpp — THE ASSIGNMENT TASK.   Write your implementation here.
// Logarithmic merging, IIR Figure 4.7 (LMERGEADDTOKEN).
//
// Available to you (see dynamic.hpp, memory_index.hpp and merge.hpp):
//   z0_              the in-memory auxiliary index (MemoryIndex):
//                    z0_.write_to(base), z0_.lookup(term), z0_.clear(), z0_.empty()
//   new_temp_name()  a fresh base path inside the work directory
//   merge_indexes()  the provided k-way merge: use it for every merge
//   readers_         ReaderCache; disk_lookup(readers_, base, term) reads one list.
//                    Call readers_.forget(base) BEFORE remove_index / rename_index
//   union_postings(a, b), remove_index(base), rename_index(from, to),
//   write_empty_index(base)
//   stats_.merges    increment it after every call to merge_indexes
// stats_.flushes and stats_.max_disk_indexes are updated for you.
//
// src/aux_main.cpp implements the auxiliary + main index with the same
// helpers. Read it first: it is a complete example of a DynamicIndex.
#include "ir/dynamic.hpp"

#include <stdexcept>

namespace ir {

LogarithmicIndex::~LogarithmicIndex() {
   
    for (const auto& generation : gen_) {
        if (!generation.empty()) {
            readers_.forget(generation);
            remove_index(generation);
        }
    }
}

void LogarithmicIndex::flush() {
    

    if (z0_.empty()) return;

    std::string z = new_temp_name();
    z0_.write_to(z);
    z0_.clear();

    for (std::size_t i = 0; ; ++i) {
        if (i == gen_.size()) {
            gen_.push_back("");
        }

        if (gen_[i].empty()) {
            gen_[i] = z;
            return;
        }

        std::string merged = new_temp_name();
        merge_indexes({ gen_[i], z }, merged);
        stats_.merges++;

        readers_.forget(gen_[i]);
        readers_.forget(z);
        remove_index(gen_[i]);
        remove_index(z);

        gen_[i].clear();
        z = merged;
    }

}

PostingsList LogarithmicIndex::lookup(const std::string& term) const {
    
    PostingsList result = z0_.lookup(term);

    for (const auto& generation : gen_) {
        if (!generation.empty()) {
            result = union_postings(
                result,
                disk_lookup(readers_, generation, term)
            );
        }
    }

    return result;
}

void LogarithmicIndex::finalize(const std::string& out_base) {
    
    flush();

    std::vector<std::string> existing;
    for (const auto& generation : gen_) {
        if (!generation.empty()) {
            existing.push_back(generation);
        }
    }

    if (existing.empty()) {
        write_empty_index(out_base);
        return;
    }

    if (existing.size() == 1) {
        readers_.forget(existing[0]);
        rename_index(existing[0], out_base);
        gen_.clear();
        return;
    }

    merge_indexes(existing, out_base);
    stats_.merges++;

    for (const auto& generation : existing) {
        readers_.forget(generation);
        remove_index(generation);
    }

    gen_.clear();
}

std::size_t LogarithmicIndex::disk_indexes() const {
    
    std::size_t count = 0;

    for (const auto& generation : gen_) {
        if (!generation.empty()) {
            ++count;
        }
    }

    return count;
}

std::uint64_t LogarithmicIndex::generation_mask() const {
    
    std::uint64_t mask = 0;

    for (std::size_t i = 0; i < gen_.size(); ++i) {
        if (!gen_[i].empty()) {
            mask |= (std::uint64_t{ 1 } << i);
        }
    }

    return mask;
}

}  // namespace ir
