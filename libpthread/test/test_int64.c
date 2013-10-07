/*
 * Copyright (c) 2011, Dongsheng Song <songdongsheng@live.cn>
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <time.h>

int main(int argc, char *argv[])
{
    __int64 x = (__int64) time(NULL);

    x *= 1000;

    printf("ms: %I64d\n", x);
    printf("ms: %I64u\n", x);
    printf("ms: %I64x\n", x);
    printf("ms: %I64X\n", x);

#if _MSC_VER >= 1500
    printf("ms: %lld\n", x);
    printf("ms: %llu\n", x);
    printf("ms: %llx\n", x);
    printf("ms: %llX\n", x);
#endif

    return 0;
}
