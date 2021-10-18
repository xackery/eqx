#include <vector>

using namespace std;
void parse(const char* path);
void pfsExtract(const char *path);
void pfsCompress(const char *path);
void bspToWld(const char* path);
vector<char> readFile(const char *filename);
void writeFile(const char* filename, vector<char> data);