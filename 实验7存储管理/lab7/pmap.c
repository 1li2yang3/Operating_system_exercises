#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_maps(const char *pid, int extended) {
    char maps_path[256];
    char smaps_path[256];
    FILE *maps_file, *smaps_file;
    snprintf(maps_path, sizeof(maps_path), "/proc/%s/maps", pid);//构建路径
    snprintf(smaps_path, sizeof(smaps_path), "/proc/%s/smaps", pid);//

    maps_file = fopen(maps_path, "r");//打开对应文件
    if (!maps_file) {
        perror("Failed to open maps file");
        return;
    }

    if (extended) {//将详细文件打开
        smaps_file = fopen(smaps_path, "r");
        if (!smaps_file) {
            perror("Failed to open smaps file");
            fclose(maps_file);
            return;
        }
    }

    char line[512];
    while (fgets(line, sizeof(line), maps_file)) {//从文件中读取
        char address[64], perms[8], path[256] = "[anonymous]";
        unsigned long offset, inode, dev_major, dev_minor;
        sscanf(line, "%63s %7s %lx %lx:%lx %lu %255s",
               address, perms, &offset, &dev_major, &dev_minor, &inode, path);

        printf("Address: %s, Perms: %s, Offset: %lx, Path: %s\n",
               address, perms, offset, path);

        if (extended) {//详细信息
            char smaps_line[512];
            while (fgets(smaps_line, sizeof(smaps_line), smaps_file)) {
                if (strncmp(smaps_line, "Size:", 5) == 0 ||
                    strncmp(smaps_line, "Rss:", 4) == 0 ||
                    strncmp(smaps_line, "Shared_Clean:", 13) == 0) {
                    printf("  %s", smaps_line);
                }
                if (smaps_line[0] == '\n') break;
            }
        }
    }

    fclose(maps_file);
    if (extended) fclose(smaps_file);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {//
      	  fprintf(stderr, "Usage: %s <pid> [-x|-X]\n", argv[0]);
      	  return 1;
    	}
    int extended = 0;
    if (argc == 3) {//如果为pmap pid -X则extended为1
    	if (strcmp(argv[2], "-x") == 0) {
            extended = 0;
        }
        if (strcmp(argv[2], "-X") == 0) {
            extended = 1;
        }
    }

    print_maps(argv[1], extended);
    return 0;
}

