#pragma once

#include "NetworkData.h"

class DataPair;

void compress(DataPair* in, DataPair** out);

void decompress(DataPair* in, DataPair** out);