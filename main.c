#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/utsname.h>
#include <sys/sysinfo.h>
#include <sys/stat.h>

int main(void) {

    // Add gap at the beginning
    printf("\n");

    char *username = getenv("USER");
    if (username == NULL) {
        username = getlogin();
    }

    // Get hostname
    char hostname[1024];
    FILE *hostname_file = fopen("/etc/hostname", "r");
    if (hostname_file == NULL) {
        perror("Failed to open /etc/hostname");
        return 1;
    }
    fgets(hostname, sizeof hostname, hostname_file);
    fclose(hostname_file);
    
    char *newline = strchr(hostname, '\n');
    if (newline) *newline = '\0';

    // Set text to bold white
    printf("\033[1;37m");

    if (username != NULL) {
        printf("    %s@%s\n", username, hostname);
    } else {
        printf("    UnknownUser@%s\n", hostname);
    }

    // Get os-release           
    char os_release[1024];
    char os_name[1024] = "(unknown)";
    FILE *os_release_file = fopen("/etc/os-release", "r");
    if (os_release_file == NULL) {
        perror("Failed to open /etc/os-release");
        return 1;
    }

    // Read through the file line by line
    while (fgets(os_release, 1024, os_release_file)) {
        if (strstr(os_release, "PRETTY_NAME=") == os_release) {
            // Extract the value after PRETTY_NAME=
            char *start = os_release + 12;  // Skip "PRETTY_NAME="
            char *end = strchr(start, '\n');
            if (end) *end = '\0';
            
            // Remove quotes if present
            if (start[0] == '"') start++;
            if (start[strlen(start)-1] == '"') start[strlen(start)-1] = '\0';
            
            strcpy(os_name, start);
            break;
        }
    }
    fclose(os_release_file);

    printf("    OS: %s\n", os_name);

    // Kernel version
    struct utsname uname_data;
    if (uname(&uname_data) == 0) {
        printf("    Kernel: %s\n", uname_data.release);
    } else {
        printf("    Kernel: Unknown\n");
    }

    // Get uptime and memory info
    struct sysinfo sys_info;
    if (sysinfo(&sys_info) == 0) {
        // Print uptime
        printf("    Uptime: %ld days, %ld hours, %ld minutes\n", 
               sys_info.uptime / 86400, 
               (sys_info.uptime % 86400) / 3600, 
               (sys_info.uptime % 3600) / 60);
        
        // Print memory info
        long total_ram = sys_info.totalram * sys_info.mem_unit / (1024 * 1024);
        long free_ram = sys_info.freeram * sys_info.mem_unit / (1024 * 1024);
        printf("    Memory: %ld MB / %ld MB\n", free_ram, total_ram);
    } else {
        printf("    Uptime: Unknown\n");
        printf("    Memory: Unknown\n");
    }

    // Reset text color before rainbow dots
    printf("\033[0m");

    // Rainbow ● dots using ANSI escape codes
    printf("        \033[0;31m● \033[0m"); // Red
    printf("\033[0;91m● \033[0m"); // Orange (Bright Red)
    printf("\033[0;33m● \033[0m"); // Yellow
    printf("\033[0;32m● \033[0m"); // Green
    printf("\033[0;34m● \033[0m"); // Blue
    printf("\033[0;35m● \033[0m"); // Indigo (Magenta)
    printf("\033[0;95m● \033[0m"); // Violet (Bright Magenta)

    // Add gap at the end
    printf("\n\n");

    return 0;
}

