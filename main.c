/*  sdl-grapher - SDL library
 *  Copyright (C) 2009 - Hans-Kristian Arntzen
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.  *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 */

#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>
#include <float.h>
#include "sdlgraph.h"

const int NUM_SAMPLES = 1600;
const int BUF_SIZE = 1<<20;
const int MAX_BLOCKS = 1<<20;

void computeEntropy(int fd, float *minEntropy);
void graphEntropy(float *minEntropy);

int main(int argc, const char **argv) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <file or device path>\n"
      "\tGraphs information entropy\n", argv[0]);
    exit(1);
  } 

  int fd = open(argv[1], O_RDONLY);
  if (fd < 0) {
    fprintf(stderr, "Couldn't open %s\n", argv[1]);
    exit(1);
  }

  float minEntropy[NUM_SAMPLES];

  computeEntropy(fd, minEntropy);

  graphEntropy(minEntropy);

  return 0;
}


void computeEntropy(int fd, float *minEntropy) {
  int counts[256];
  uint8_t buf[BUF_SIZE];
  ssize_t totalBytesRead = 0;
  float blockEntropy[MAX_BLOCKS];
  float minEntropySoFar = FLT_MAX;

  // Read the file, compute entropy as we go
  int block;
  for (block = 0;; block++) {
    // Read the next buffer from the file descriptor
    ssize_t bytesRead = read(fd, buf, BUF_SIZE);
    if (bytesRead == (ssize_t)-1) break;
    totalBytesRead += bytesRead;
    if (bytesRead < 100) break; // Ignore last bit, if too small
    
    // Compute entropy, normalized to [0, 1)
    memset(counts, 0, sizeof(int)*256);
    for (ssize_t i = 0; i < bytesRead; i++) {
      counts[buf[i]]++;
    }
    float entropy = 0.0f;
    for (int i = 0; i < 256; i++) {
      if (counts[i] == 0) continue;
      float p = (float)counts[i] / (float)bytesRead;
      entropy -= p*log2f(p);
    }
    entropy /= 8.0f; // = log2f(256.0f)
    blockEntropy[block] = entropy;
    if (entropy < minEntropySoFar) {
      minEntropySoFar = entropy;
    }

    // Show progress 
    if ((block + 1) % 100 == 0) {
      printf("Processed %zdMB, min entropy so far: %f\n",
        totalBytesRead >> 20, minEntropySoFar);
    }
  }
  int numBlocks = block;

  printf("Done. Processed %zdMB, min entropy: %f\n",
    totalBytesRead >> 20, minEntropySoFar);

  // Compute the lowest entropy in each sample (slice of the file)
  for (int i = 0; i < NUM_SAMPLES; i++) {
    minEntropy[i] = FLT_MAX;
  }
  for (block = 0; block < numBlocks; block++) {
    int sample = block * NUM_SAMPLES / numBlocks;
    if (blockEntropy[block] < minEntropy[sample]) {
      minEntropy[sample] = blockEntropy[block];
    }
  }
}

void graphEntropy(float *minEntropy) {
  Graph myGraph;
  myGraph.xMin = -1;
  myGraph.xMax = 101.0;
  myGraph.xScale = 10.0;
  myGraph.yMin = -0.01;
  myGraph.yMax = 1.01;
  myGraph.yScale = 0.1;
  myGraph.width = 800;
  myGraph.height = 400;
  
  init_graph(&myGraph); /* Initialize the graph, creating a new SDL window */
  
  set_color(&myGraph, 0x55, 0x55, 0x55); /* Draw axes and gridlines */
  draw_grid(&myGraph, 0, 2); 

  set_color(&myGraph, 0, 0xFF, 0); /* Draw plot points */
  for (int i = 0; i < NUM_SAMPLES; i++) {
    float x = (float)i / (float)NUM_SAMPLES * 100.0f;
    float y = (float)minEntropy[i];
    print_pixel(&myGraph, x, y);
  }
  update_screen(&myGraph);

  idle();   /* Wait for the user to close the window */
  quit();
}
