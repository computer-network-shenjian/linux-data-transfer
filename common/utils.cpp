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

    // 3. register a proper signal handler
    // SIGUSR1 is all we need for this project
    signal(SIGUSR1, signal_handler);

    return shared_memory;
}

int allocate_shared_memory(int shared_segment_size) {
    return shmget (IPC_PRIVATE, shared_segment_size,
            IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
}

int deallocate_shared_memory(int segment_id) {
    return shmctl (segment_id, IPC_RMID, 0);
}

void write_bytes_to_file(string fn, uint8_t *ptr_bytes, int num_bytes) {
    ofstream ofile(fn, ios::binary);
    ofile.write(ptr_bytes, num_bytes);
    ofile.close();
}

ErrorCode log_init(std::ofstream &log_stream, const std::string log_name, const Level level) {
    // log_stream must not be opened before calling this function.
    if (log_stream.is_open()) return ErrorCode::LogInit;

    log_stream.open(log_name, ios::out|ios::trunc);
    if (!log_stream.is_open()) return ErrorCode::LogInit;

    Log::get().setLogStream(log_stream);
    Log::get().setLevel(level);
    return ErrorCode::OK;
}
