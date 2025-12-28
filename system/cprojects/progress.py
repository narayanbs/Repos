import time  # for simulating download delay
import sys   # for controlling terminal output

# Total file size in bytes (1 GB)
total_size = 1_073_741_824  

# How many bytes we "download" per iteration (10 MB)
chunk_size = 10_485_760  

# Counter for bytes downloaded so far
bytes_downloaded = 0

# Loop until the file is fully downloaded
while bytes_downloaded < total_size:
    time.sleep(0.1)  # simulate network delay for each chunk
    bytes_downloaded += chunk_size  # increment downloaded bytes
    
    # Ensure we don't exceed the total size
    if bytes_downloaded > total_size:
        bytes_downloaded = total_size

    # Calculate progress as a percentage
    progress = bytes_downloaded / total_size * 100

    # Display progress on the same line
    # \r moves cursor to the start of the line
    # end="" prevents adding a new line
    sys.stdout.write(f"\rProgress: {progress:.2f}%")
    sys.stdout.flush()  # forces immediate output

print("\nDownload complete!")  # new line after finishing

