/*
 * Simulating a progress bar in C
 */
#include <stdio.h>
#include <unistd.h>  // for usleep()

int main() {
  // Total file size in bytes (1 GB)
  long total_size = 1073741824;

  // Bytes downloaded per iteration (10 MB)
  long chunk_size = 10485760;

  // Counter for bytes downloaded so far
  long bytes_downloaded = 0;

  // Loop until the file is fully downloaded
  while (bytes_downloaded < total_size) {
    usleep(100000);                  // simulate network delay (0.1 sec)
    bytes_downloaded += chunk_size;  // increment downloaded bytes

    // Make sure we do not exceed total_size
    if (bytes_downloaded > total_size) {
      bytes_downloaded = total_size;
    }

    // Calculate progress as a percentage
    double progress = (double)bytes_downloaded / total_size * 100;

    // Print progress on the same line
    // \r moves cursor to the beginning of the line
    printf("\rProgress: %.2f%%", progress);
    fflush(stdout);  // force immediate output (important in C)
  }

  // Print a newline after completion
  printf("\nDownload complete!\n");
  return 0;
}
