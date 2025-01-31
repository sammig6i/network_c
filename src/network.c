#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <ctype.h>
#include <string.h>
#include <dirent.h>


#define MAXIFLEN 32
#define SYSCLASSNET "/sys/class/net"
#define PROCNETDEV "/proc/net/dev"

typedef struct iflist {
    char interface[MAXIFLEN];
    int64_t id;
    uint32_t bandwidth;
    struct iflist *next;
} iflist;


int isifvalid(const char *iface) {
    if (strchr(iface, ':') != NULL) {
        return 0;
    } else if (strcmp(iface, "lo") == 0) {
        return 0;
    } else if (strcmp(iface, "lo0") == 0) {
        return 0;
    } else if (strcmp(iface, "sit0") == 0) {
        return 0;
    }

    return 1;
}

int iflistadd(iflist **ifl, const char *iface, const int id) {
    iflist *newif = NULL, *ifl_iterator = *ifl;

    newif = malloc(sizeof(iflist));
    if (newif == NULL) {
        return 0;
    }

    newif->next = NULL;
    if (*ifl != NULL) {
        while (ifl_iterator->next != NULL) {
            *ifl_iterator = *ifl_iterator->next;
        }
        ifl_iterator->next = newif;
    } else {
        *ifl = newif;
    }

    strncpy(newif->interface, iface, MAXIFLEN);
    newif->id = id;
    return 1;
}


int getiflist(iflist **ifl, const int validate) {
    FILE *fp;
    DIR *dp;
    const struct dirent *di;
    char procline[512];
    char temp[64];
    char interface[MAXIFLEN];

    if ((fp = fopen(PROCNETDEV, "r")) != NULL) {
        while (fgets(procline, 512, fp) != NULL) {
            sscanf(procline, "%63s", temp);
            if (strlen(temp) > 0 && (isdigit(temp[strlen(temp - 1)]) || temp[strlen(temp) - 1] == ':')) {
                sscanf(temp, "%31[^':']s", interface);
                if (validate && !isifvalid(interface)) {
                    continue;
                }

                iflistadd(ifl, interface, 0);
            }
        }
        fclose(fp);
        return 1;
    } else {
        if ((dp = opendir(SYSCLASSNET)) != NULL) {
            while ((di = readdir(dp))) {
                if (di->d_name[0] == '.' || strlen(di->d_name) > MAXIFLEN - 1) {
                    continue;
                }
                if (validate && !isifvalid(di->d_name)) {
                    continue;
                }
                iflistadd(ifl, di->d_name, 0);
            }
            closedir(dp);
            return 1;
        }
    }

    return 0;
}


int main(int argc, char *argv[]) {
    iflist *ifl = NULL;
    int ifcount;

    ifcount = getiflist(&ifl, 1);
    if (ifcount) {
        printf("Extracted interface: %s\n", ifl->interface);
    } else {
        printf("No interface found.\n");
    }

    // show interface data here
    /* VnStat:
        handlesetalias(&p);
        handleshowdata(&p);
        handletrafficmeters(&p);
    */

    // free the memory held for interface linked list 
    iflist *ifl_prev = NULL;
    while (ifl != NULL) {
        ifl_prev = ifl;
        ifl = ifl->next;
        free(ifl_prev);
    }
    return 0;

}