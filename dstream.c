/**
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "lz4.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int write_decompressed_segment(LZ4_streamDecode_t *stream,
                                    FILE *ifp, FILE *ofp)
{
    size_t buf_size, res;
    char buf[512];
    int dsize;
    char obuf[16384];

    buf_size = fread(buf, 1, sizeof(buf), ifp);
    if (buf_size <= 0) {
        if (feof(ifp)) {
            fprintf(stderr, "EOF.\n");
            return 0;
        }
        int err = errno;
        fprintf(stderr, "fread failed.  error %s (%d)\n",
                strerror(err), err); 
        return -1;
    }
    fprintf(stderr, "fread read %zd bytes.\n", buf_size);
    dsize = LZ4_decompress_safe_continue(stream, buf, obuf,
                                 buf_size, sizeof(obuf));
    if (dsize <= 0) {
        fprintf(stderr, "LZ4_decompress_safe_continue error %d.\n",
                dsize);
        return -1;
    }
    fprintf(stderr, "LZ4_decompress_safe_continue decompressed "
            "%d bytes.\n", dsize);
    res = fwrite(obuf, 1, dsize, ofp);
    if (res < dsize) {
        fprintf(stderr, "fwrite failed.\n");
        return -1;
    }
    return res;
}

static int run_test(LZ4_streamDecode_t *stream, char *ipath, char *opath)
{
    FILE *ifp = NULL, *ofp = NULL;
    int ret;

    ifp = fopen(ipath, "r");
    if (!ifp) {
        int err = errno;
        fprintf(stderr, "fopen(%s, r) failed: error %s (%d)\n",
                ipath, strerror(err), err);
        return EXIT_FAILURE;
    }
    ofp = fopen(opath, "w");
    if (!ofp) {
        int err = errno;
        fclose(ifp);
        fprintf(stderr, "fopen(%s, w) failed: error %s (%d)\n",
                opath, strerror(err), err);
        return EXIT_FAILURE;
    }
    while (1) {
      int res = write_decompressed_segment(stream, ifp, ofp);
      if (res == 0) {
          ret = EXIT_SUCCESS;
          break;
      }
      if (res < 0) {
          ret = EXIT_FAILURE;
          break;
      }
    }
    fclose(ifp);
    if (fclose(ofp) < 0) {
        int err = errno;
        fprintf(stderr, "fclose(%s) failed: error %s (%d)\n",
                opath, strerror(err), err);
        ret = EXIT_FAILURE;
    }
    return ret;
}

int main(int argc, char **argv)
{
    int ret;
    LZ4_streamDecode_t *stream = NULL;

    if (argc < 3) {
        fprintf(stderr, "usage: %s [file-to-decompress] [output-file]\n",
                argv[0]);
        exit(EXIT_FAILURE);
    }
    stream = LZ4_createStreamDecode();
    if (!stream) {
        fprintf(stderr, "LZ4_createStream failed.\n");
        return EXIT_FAILURE;
    }
    ret = run_test(stream, argv[1], argv[2]);
    LZ4_freeStreamDecode(stream);
    return ret;
}

