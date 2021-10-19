#include <vector>

using namespace std;
void parse(const char* path);
void pfsExtract(const char *path);
void pfsCompress(const char *path);
void bspToWld(const char* path);
void wldToBsp(const char* path);
vector<char> readFile(const char *filename);
void writeFile(const char* filename, vector<char> data);
void gltfToWld(const char *path);
void glbToWld(const char *path);
void gltfMeshToWld(const char *path) {