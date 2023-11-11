#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define MAX 100

typedef struct {
  int x, y;
} Posicion;

typedef struct {
  int mejor_camino;
  int matrizVisitados[MAX][MAX];
} SolucionOptima;

void abrirLaberinto(char* fuente, char laberinto[MAX][MAX], int* filas, int* columnas) {
  FILE* archivo = fopen(fuente, "r");
  char linea[MAX];
  *filas = 0;
  while (fgets(linea, MAX, archivo)) {
    linea[strcspn(linea, "\r\n")] = 0;
    strcpy(laberinto[*filas], linea);
    *columnas = strlen(linea);
    (*filas)++;
  }
  fclose(archivo);
}

Posicion encontrarCaracter(char laberinto[MAX][MAX], int filas, int columnas, char caracter) {
  Posicion entrada = {-1, -1};
  for (int i = 0; i < filas; i++) {
    for (int j = 0; j < columnas; j++) {
      if (laberinto[i][j] == caracter) {
        entrada.x = i;
        entrada.y = j;
        return entrada;
      }
    }
  }
  return entrada;
}

Posicion buscarPortal(char matriz[MAX][MAX], int filas, int columnas, Posicion inicial, char letra) {
  Posicion regreso = {-1, -1};
  for (int i = 0; i < filas; i++) {
    for (int j = 0; j < columnas; j++) {
      if (matriz[i][j] == letra && !(i == inicial.x && j == inicial.y)) {
        regreso.x = i;
        regreso.y = j;
        return regreso;
      }
    }
  }
  return regreso;
}

int esPosible(int f, int c, char matriz[MAX][MAX], int filas, int columnas) {
  return (0 <= f && f < filas && 0 <= c && c < columnas && matriz[f][c] != '#');
}

int calcular_distancia(Posicion origen, Posicion salida) {
  return abs(origen.x - salida.x) + abs(origen.y - salida.y);
}

void buscarPortales(char laberinto[MAX][MAX], int filas, int columnas, Posicion portales[], int* num_portales) {
  *num_portales = 0;
  for (int i = 0; i < filas; i++) {
    for (int j = 0; j < columnas; j++) {
      if (laberinto[i][j] != '.' && laberinto[i][j] != '#' && laberinto[i][j] != 'E' && laberinto[i][j] != 'S') {
        int encontrado = 0;
        for (int k = 0; k < *num_portales; k++) {
          if (laberinto[i][j] == laberinto[portales[k].x][portales[k].y]) {
            encontrado = 1;
            break;
          }
        }
        if (encontrado) {
          portales[*num_portales].x = i;
          portales[*num_portales].y = j;
          (*num_portales)++;
        } else {
          portales[*num_portales].x = i;
          portales[*num_portales].y = j;
          (*num_portales)++;
        }
      }
    }
  }
}

int distanciaPortal(Posicion origen, Posicion portales[], int num_portales, Posicion salida) {
  int distancia_minima = INT_MAX;
  for (int i = 0; i < num_portales; i++) {
    int distanciaTotal = calcular_distancia(origen, portales[i]) + calcular_distancia(portales[i], portales[(i+1)%num_portales]) + fmin(calcular_distancia(portales[i], salida), calcular_distancia(portales[(i+1)%num_portales], salida));
    if (distanciaTotal < distancia_minima) {
      distancia_minima = distanciaTotal;
    }
  }
  return distancia_minima;
}

void exportarSolucion(char laberinto[MAX][MAX], int filas, int columnas, int matriz[MAX][MAX]) {
  FILE* archivo = fopen("laberintoSolucion.txt", "w");
  for (int i = 0; i < filas; i++) {
    for (int j = 0; j < columnas; j++) {
      if (matriz[i][j] == 1 && laberinto[i][j] == '.') {
        fprintf(archivo, "X");
      } else {
        fprintf(archivo, "%c", laberinto[i][j]);
      }
    }
    fprintf(archivo, "\n");
  }
  fclose(archivo);
}

void solucion(int f, int c, char laberinto[MAX][MAX], int filas, int columnas, int matrizVis[MAX][MAX], int contador, SolucionOptima* mejor_solucion, Posicion portales[], int num_portales) {
  if ((contador > mejor_solucion->mejor_camino) && mejor_solucion->mejor_camino != -1) {
    return;
  }
  if (laberinto[f][c] == 'S') {
    if (contador < mejor_solucion->mejor_camino || mejor_solucion->mejor_camino == -1) {
      mejor_solucion->mejor_camino = contador;
      memcpy(mejor_solucion->matrizVisitados, matrizVis, sizeof(int)*MAX*MAX);
    }
    return;
  }

  matrizVis[f][c] = 1;

  if (laberinto[f][c] != '#' && laberinto[f][c] != '.' && laberinto[f][c] != 'E' && laberinto[f][c] != 'S') {
    Posicion otroPortal = buscarPortal(laberinto, filas, columnas, (Posicion){f, c}, laberinto[f][c]);
    if (otroPortal.x != -1 && matrizVis[otroPortal.x][otroPortal.y] != 1) {
      char letraPortal = laberinto[f][c];
      laberinto[f][c] = '.';
      laberinto[otroPortal.x][otroPortal.y] = '.';
      solucion(otroPortal.x, otroPortal.y, laberinto, filas, columnas, matrizVis, contador + 1, mejor_solucion, portales, num_portales);
      laberinto[f][c] = letraPortal;
      laberinto[otroPortal.x][otroPortal.y] = letraPortal;
    }
  }

  int movimientos[4][2] = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};
  for (int i = 0; i < 4; i++) {
    if (esPosible(f + movimientos[i][0], c + movimientos[i][1], laberinto, filas, columnas) && matrizVis[f + movimientos[i][0]][c + movimientos[i][1]] != 1) {
      solucion(f + movimientos[i][0], c + movimientos[i][1], laberinto, filas, columnas, matrizVis, contador + 1, mejor_solucion, portales, num_portales);
    }
  }

  matrizVis[f][c] = 0;
}

void main() {
  char laberinto[MAX][MAX];
  int filas, columnas;
  abrirLaberinto("laberinto.txt", laberinto, &filas, &columnas);
  Posicion entrada = encontrarCaracter(laberinto, filas, columnas, 'E');
  int matrizVisitados[MAX][MAX] = {0};
  SolucionOptima mejor_solucion = {-1, {{0}}};
  Posicion portales[MAX];
  int num_portales;
  buscarPortales(laberinto, filas, columnas, portales, &num_portales);
  solucion(entrada.x, entrada.y, laberinto, filas, columnas, matrizVisitados, 0, &mejor_solucion, portales, num_portales);
  printf("Mejor solución: %d\n", mejor_solucion.mejor_camino);
  exportarSolucion(laberinto, filas, columnas, mejor_solucion.matrizVisitados);
}
