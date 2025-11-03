#include "http.h"
#include "router.h"
#include "db.h"
#include "render.h"
#include "admin.h"
#include "board.h"
#include "upload.h"
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#define DEFAULT_PORT 8080
#define DEFAULT_DB_PATH "app.db"

static volatile int keep_running = 1;

void signal_handler(int signum) {
    (void)signum;
    keep_running = 0;
}

int main(int argc, char *argv[]) {
    uint16_t port = DEFAULT_PORT;
    const char *db_path = DEFAULT_DB_PATH;
    
    printf("=== Cosmopolitan Web Application ===\n");
    printf("Build: %s %s\n", __DATE__, __TIME__);
    printf("Starting initialization...\n\n");
    
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    
    if (db_init(db_path) != 0) {
        fprintf(stderr, "Failed to initialize database\n");
        return 1;
    }
    
    if (db_migrate() != 0) {
        fprintf(stderr, "Failed to run database migrations\n");
        db_close();
        return 1;
    }
    
    router_init();
    
    board_init();
    board_register_routes();
    
    admin_init();
    admin_register_routes();
    
    upload_init("./uploads");
    upload_register_routes();
    
    if (http_server_init(port) != 0) {
        fprintf(stderr, "Failed to initialize HTTP server\n");
        router_cleanup();
        db_close();
        return 1;
    }
    
    printf("\n");
    printf("Server ready!\n");
    printf("Listening on: http://localhost:%u\n", port);
    printf("Press Ctrl+C to stop\n\n");
    
    http_server_run();
    
    printf("\nShutting down...\n");
    http_server_shutdown();
    router_cleanup();
    db_close();
    
    printf("Shutdown complete\n");
    return 0;
}
