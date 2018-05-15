#include "Huffman.h"

/// <summary>
/// Compress data using Huffman method
/// </summary>
/// <param name="raw">Input, no need to delete</param>
/// <param name="compact">Result. Must be deleted when done</param>
void DataCompress(DataPair* raw, DataPair** compact) {
  *compact = raw;
}

/// <summary>
/// Compress data using Huffman method
/// </summary>
/// <param name="compact">Input, no need to delete</param>
/// <param name="raw">Result. Must be deleted when done</param>
void DataExtract(DataPair* compact, DataPair** raw) {
  *raw = compact;
}