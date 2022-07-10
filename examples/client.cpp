#include <cat/array>
#include <cat/linux>
#include <cat/memory>
#include <cat/socket>
#include <cat/string>

int main(int argc, char* p_argv[]) {
    cat::SocketUnix<cat::SocketType::stream> socket;
    socket.path_name = cat::StaticString<108>::padded("\0/tmp/temp.sock");

    socket.create().or_panic();
    socket.connect().or_panic();

    // Send all command line arguments to the server.
    for (int i = 1; i < argc; ++i) {
        socket.send_string(p_argv[i]).or_panic();
        if (i < argc - 1) {
            // TODO: What does this flag `0b1` mean?
            socket.send_string(" ", 0b1).or_panic();
        }
    }

    socket.close().or_panic();
    cat::exit();
}
