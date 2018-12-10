#include "utils.hpp"

template<std::size_t N>
void generate_init_vector(uint8_t (&IV_buff)[N]) {
    using bytes_randomizer = std::independent_bits_engine<std::default_random_engine, CHAR_BIT, uint8_t>;
    std::default_random_engine rd;
    bytes_randomizer bytes(rd);

    std::generate(std::begin(IV_buff), std::end(IV_buff), std::ref(bytes));
}

char* process_init(int segment_id) {
    // 1. Register death of child when parent dies
    prctl(PR_SET_PDEATHSIG, SIGHUP);

    // 2. Attach shared memory specified by segment_id and return its address
    char *shared_memory = (char*) shmat(segment_id, nullptr, 0);

    // 3. register proper signal handlers
    // SIGUSR1 and SIGUSR2 are all we need for this project
    signal(SIGUSR1, signal_handler);
    signal(SIGUSR2, signal_handler);

    return shared_memory;
}

int allocate_shared_memory(int shared_segment_size) {
    return shmget (IPC_PRIVATE, shared_segment_size,
            IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
}

int deallocate_shared_memory(int segment_id) {
    return shmctl (segment_id, IPC_RMID, 0);
}
