#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>


#define _DEFAULT_SOURCE
#define _POSIX_C_SOURCE 200112L
#define _XOPEN_SOURCE 600
#define S_ISVTX 001000
#define S_IFSOCK 140000
short int lnk =0;
struct stat sb;
DIR *d;
struct dirent *dir;

//permet de déterminer le type de fichier 
// void typeDeFichier(struct stat sb){
//     switch (sb.st_mode & S_IFMT) {
//         case S_IFBLK:  printf("b");/*périphérique bloc*/     break;
//         case S_IFCHR:  printf("c");/*périphérique caractère*/  break;
//         case S_IFLNK:  printf("l");/*lien symbolique*/         break;
//         case S_IFDIR:  printf("d");/*répertoire*/              break;
//         case S_IFIFO:  printf("p");/*FIFO/ube*/               break;
//         case S_IFREG:  printf("-");/*fichier ordinaire*/      break;
//         case S_IFSOCK: printf("s");/*socket*/                  break;
//         default:       printf("?\n");               break;
//     }
// }

void typeDeFichier(){
    if( S_ISLNK (sb.st_mode)){
        printf("l");//créer un booléen qui est vraie si c'est un lien symbolique
        lnk = 1;
    }
    else if( S_ISREG(sb.st_mode)){
        printf("-");
    }
    else if( S_ISCHR(sb.st_mode)){
        printf("c");
    }
    else if( S_ISBLK(sb.st_mode)){
        printf("b");
    }
    else if( S_ISFIFO(sb.st_mode)){
        printf("p");
    }
    
    else if( S_ISDIR(sb.st_mode)){
        printf("d");
    }
    else if( S_IFSOCK & sb.st_mode){
        printf("s");
    }
    
}

// void typeDeFichier (){
//     if(DT_BLK==dir->d_type){
//         printf("b");
//     }else if (DT_CHR==dir->d_type){
//         printf("c");
//     }else if (DT_DIR==dir->d_type){
//         printf("d");
//     }else if (DT_FIFO==dir->d_type){
//         printf("p");
//     }else if (DT_LNK==dir->d_type){
//         printf("l");
//         lnk=1;
//     }else if (DT_SOCK==dir->d_type){
//         printf("s");
//     }else if (DT_REG==dir->d_type){
//         printf("-");
//     }
// }



void permission (){ // fonction qui permet d'identifier les permissions avec les bits spéciaux.
// 
    // S_ISBLK & sb.st_mode
    // if(S_ISBLK && )
    if(S_IRUSR & sb.st_mode){
        printf("r");
    }else{
        printf("-");
    }
    if(S_IWUSR & sb.st_mode){
        printf("w");
    }else{
        printf("-");
    }
    if (S_ISUID & sb.st_mode){
        printf("s");
    }
    else if(S_IXUSR & sb.st_mode){
        printf("x");
    }else{
        printf("-");
    }
    if(S_IRGRP & sb.st_mode){
        printf("r");
    }else{
        printf("-");
    }
    if(S_IWGRP & sb.st_mode){
        printf("w");
    }else{
        printf("-");
    }
    if(S_ISGID & sb.st_mode){
        printf("s");
    } 
    else if(S_IXGRP & sb.st_mode){
        printf("x");
    }else{
        printf("-");
    }
    if(S_IROTH & sb.st_mode){
        printf("r");
    }else{
        printf("-");
    }
    if(S_IWOTH & sb.st_mode){
        printf("w");
    }else{
        printf("-");
    }if(S_ISVTX & sb.st_mode){
        printf ("t ");
    }else if(S_IXOTH & sb.st_mode){
        printf("x ");
    }else{
        printf("- ");
    }   
}

void autorisation(struct stat sb){ // fonction intermédiaire.
    typeDeFichier(sb);
    permission(sb);
}


void infoFichier (struct stat *sb){
    autorisation(*sb);
    printf("%ld ", (long) sb->st_nlink); // affiche le nb lien

    printf("%ld %ld ",(long) sb->st_uid, (long) sb->st_gid); //affiche l'UID puis GID , autorisation

    printf("%lld ",(long long) sb->st_size);//taille du doc

    struct tm * timeInfos= gmtime(&sb->st_mtime); //tmp
    printf("%i-%i-%i %i:%i ",timeInfos->tm_year+1900,timeInfos->tm_mon+1,timeInfos->tm_mday, timeInfos->tm_hour+1, timeInfos->tm_min);//affiche la date dans le formats vous 

}

void cheminLNK(){ // fct qui permet de récupérer et d'affichier le chemin d'accés d'un lien symbolique
    char *buf;
    ssize_t nbytes, bufsiz;
    bufsiz = sb.st_size + 1;
    if (sb.st_size == 0){
        bufsiz = 4096;
    }
    buf = (char*)calloc(bufsiz,sizeof(char));
    if (buf == NULL) {
    perror("calloc");
    exit(EXIT_FAILURE);
    }

    nbytes = readlink(dir->d_name, buf, bufsiz);
    if (nbytes == -1) {
        perror("readlink");
        exit(EXIT_FAILURE);
    }
    printf(" -> %s\n",buf);
    free(buf);  
}
void dire (char* argv){
    d=opendir(argv);
    char *resolved_path=malloc(4096*sizeof(char*));//4096 car c'est la valeur de PATH_MAX dans la page du manuelle concernant realpath
    //printf("%s",dir->d_name);
    errno=0;
    if (d)
    {
        while ((dir = readdir(d)))
        {
            lstat(dir->d_name, &sb);
            if (errno!=0){
                perror("lstat");
            }
            // printf("\n%d<-\n",errno);
            infoFichier(&sb);
            if (!(lnk)){
                printf("%s\n", dir->d_name);
            }else{
                printf("%s",dir->d_name);
                // realpath(dir->d_name,resolved_path);
                cheminLNK();
                // printf(" -> %s\n",resolved_path);
                lnk=0;
            }
            // printf("\n");
        }
        free(resolved_path);
        closedir(d);
    }
}

int main(void)
{
    dire(".");
    return 0;
}

//ls -aln --time-style=long-iso