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

static int run_test_impl(FILE *ifp, FILE *ofp)
{
    size_t buf_size, res;
    char buf[8192];
    int worst_case, actual_case;
    char obuf[32000];

    buf_size = fread(buf, 1, sizeof(buf), ifp);
    if (buf_size < 0) {
        int err = errno;
        fprintf(stderr, "fread failed.  error %s (%d)\n",
                strerror(err), err); 
        return EXIT_FAILURE;
    }
    fprintf(stderr, "fread read %zd bytes.\n", buf_size);
    worst_case = LZ4_compressBound(buf_size);
    fprintf(stderr, "worst case for compressing these bytes is %d.\n",
            worst_case);
    if (worst_case > sizeof(obuf)) {
        fprintf(stderr, "buffer too small.\n");
        return EXIT_FAILURE;
    }
    actual_case = LZ4_compress(buf, obuf, buf_size);
    if (actual_case == 0) {
        fprintf(stderr, "compression failed.\n");
        return EXIT_FAILURE;
    }
    fprintf(stderr, "compressed down to %d bytes.\n", actual_case);
    res = fwrite(obuf, 1, actual_case, ofp);
    if (res < actual_case) {
        fprintf(stderr, "fwrite failed.\n");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

static int run_test(char *ipath, char *opath)
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
    ret = run_test_impl(ifp, ofp);
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
    if (argc < 3) {
        fprintf(stderr, "usage: %s [file-to-compress] [output-file]\n",
                argv[0]);
        exit(EXIT_FAILURE);
    }
    return run_test(argv[1], argv[2]);
}
