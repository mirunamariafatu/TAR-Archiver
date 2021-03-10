#include <stdio.h>
#define N 60
#include <stdlib.h>
#include <string.h>

union record {
  char charptr[512];
  struct header {
    char name[100];
    char mode[8];
    char uid[8];
    char gid[8];
    char size[12];
    char mtime[12];
    char chksum[8];
    char typeflag;
    char linkname[100];
    char magic[8];
    char uname[32];
    char gname[32];
    char devmajor[8];
    char devminor[8];
  } header;
};

long rotunjireMarime(long marime) {
  while (marime % 512 != 0) {
    marime++;
  }
  return marime;
}

long marimeFisier(FILE *fisier) {
  fseek(fisier, 0, SEEK_END);
  long marime = ftell(fisier);
  fseek(fisier, 0, SEEK_SET);
  return marime;
}

void arch(char **nume, int nrFisiere, char *numeArhiva) {
  FILE *fisier1 = fopen(numeArhiva, "w");
  for (int j = 0; j < nrFisiere; j++) {
    union record tema;
    FILE *fisier = fopen(nume[j], "r");
    if (fisier == NULL) {
      continue;
    }
    long marime = marimeFisier(fisier);
    long marimeRot = rotunjireMarime(marime);
    for (int i = 0; i < 512; i++) {
      tema.charptr[i] = 0;
    }

    strcpy(tema.header.name, nume[j]);

    strcpy(tema.header.mode, "100644");

    strcpy(tema.header.uid, "0001000");

    strcpy(tema.header.gid, "0001000");

    sprintf(tema.header.size, "%011lo", marime);

    sprintf(tema.header.mtime, "%011o", 0);

    strcpy(tema.header.uname, "ana");

    strcpy(tema.header.gname, "ana");

    strcpy(tema.header.magic, "GNUtar ");

    tema.header.typeflag = '0';
    tema.header.linkname[0] = '0';
    tema.header.chksum[1] = ' ';
    tema.header.chksum[2] = ' ';
    tema.header.chksum[3] = ' ';
    tema.header.chksum[4] = ' ';
    tema.header.chksum[5] = ' ';
    tema.header.chksum[6] = ' ';
    tema.header.chksum[7] = ' ';
    tema.header.chksum[0] = ' ';

    int chksum = 0;
    for (int i = 0; i < 512; i++) chksum += tema.charptr[i];

    sprintf(tema.header.chksum, "%07o", chksum);
    tema.header.chksum[7] = ' ';

    fwrite(&tema, sizeof(union record), 1, fisier1);
    char *continut = calloc(marimeRot, sizeof(char));
    for (int i = 0; i < marimeRot; i++) continut[i] = 0;
    fread(continut, sizeof(char), marime, fisier);
    fwrite(continut, sizeof(char), marimeRot, fisier1);
    free(continut);
    fclose(fisier);
  }
  union record gol;
  for (int i = 0; i < 512; i++) gol.charptr[i] = 0;
  fwrite(&gol, sizeof(union record), 1, fisier1);
  fclose(fisier1);
}

void create(char *nume) {
  int fisiere = 0;
  FILE *files = fopen("files.txt", "r");
  char n[200] = {0};
  char fisier[200];
  while (!feof(files)) {
    fgets(fisier, 199, files);
    fisiere++;
  }

  fseek(files, 0, SEEK_SET);
  char **fis = calloc(fisiere, sizeof(char *));
  for (int i = 0; i < fisiere; i++) {
    fis[i] = calloc(50, sizeof(char));
  }

  int contor = 0;
  while (!feof(files)) {
    // fgets(v, 199, files);
    fscanf(files, "%s %s %s %s %s %s %s %s %s\n", n, n, n, n, n, n, n, n,
          fisier);
    strcpy(fis[contor], fisier);
    contor++;
  }
  arch(fis, fisiere, nume);
  printf("%s\n", "> Done!");
  for (int i = 0; i < fisiere; i++) {
    free(fis[i]);
  }
  free(fis);
}

void list(char *numeArhiva) {
  union record listare;
  FILE *arhiva = fopen(numeArhiva, "r");
  while (!feof(arhiva)) {
    fread(&listare, sizeof(union record), 1, arhiva);
    if (listare.header.name[0] != '\0') printf("> %s\n", listare.header.name);
    long lungime = 0;
    sscanf(listare.header.size, "%lo", &lungime);
    lungime = rotunjireMarime(lungime);
    for (int i = 0; i < lungime; ++i) {
      fgetc(arhiva);
    }
  }
  fclose(arhiva);
}

void extract(char *numeFisier, char *numeArhiva) {
  FILE *arhiva = fopen(numeArhiva, "r");
  while (!feof(arhiva)) {
    union record listare;
    fread(&listare, sizeof(union record), 1, arhiva);
    if (strcmp(listare.header.name, numeFisier)) continue;
    long lungime = 0;
    sscanf(listare.header.size, "%011lo", &lungime);
    char numeNou[100] = {0};
    strcat(numeNou, "extracted_");
    strcat(numeNou, listare.header.name);
    if (!strcmp(numeNou, "extracted_")) continue;
    FILE *fisier = fopen(numeNou, "w");
    if (fisier == NULL) continue;
    long k = 0;
    long lungimeRot = rotunjireMarime(lungime);

    while (k < lungime) {
      char c = fgetc(arhiva);
      if (!strcmp(listare.header.name, numeFisier)) {
        fwrite(&c, 1, 1, fisier);
      }
      k++;
    }
    while (k < lungimeRot) {
      fgetc(arhiva);
      k++;
    }
    fclose(fisier);
  }
  printf("> File extracted!\n");
}

int main() {
  char v[N] = {0};
  while (strcmp(v, "exit")) {
    fgets(v, N - 1, stdin);
    char *comanda;
    char *nume;
    char *folder;
    v[strlen(v) - 1] = 0;
    comanda = strtok(v, " ");
    nume = strtok(NULL, " ");
    folder = strtok(NULL, " ");
    if (comanda != NULL) {
      if (!strcmp("create", comanda)) {
        create(nume);
      }
    }
    if (comanda != NULL) {
      if (!strcmp(comanda, "list")) {
        list(nume);
      }
    }
    if (comanda != NULL) {
      if (!strcmp("extract", comanda)) {
        extract(nume, folder);
      }
    }
  }
}
