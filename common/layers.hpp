#include "utils.hpp"

int sender_application_layer(const int pid, const int segment_id);
int receiver_application_layer(const int segment_id, const CopyMode copy_mode);
void signal_handler_RAL(int sig);

int sender_transport_layer(const int pid, const int segment_id, const CopyMode copy_mode);
int receiver_transport_layer(const int pid, const int segment_id, const CopyMode copy_mode);
void signal_handler_STL(int sig);
void signal_handler_RTL(int sig);

int sender_network_layer(const int pid, const int segment_id, const CopyMode copy_mode);
int receiver_network_layer(const int pid, const int segment_id, const CopyMode copy_mode);
void signal_handler_SNL(int sig);
void signal_handler_RNL(int sig);

int sender_datalink_layer(const int pid, const int segment_id, const CopyMode copy_mode);
int receiver_datalink_layer(const int pid, const int segment_id, const CopyMode copy_mode);
void signal_handler_SDL(int sig);
void signal_handler_RDL(int sig);

int sender_physical_layer(const int segment_id, const CopyMode copy_mode);
int receiver_physical_layer(const int pid, const int segment_id);
void signal_handler_SPL(int sig);
