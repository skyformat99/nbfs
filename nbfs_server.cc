/*
 *  Copyright (c) 2016, https://github.com/nebula-im
 *  All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "nbfs_server.h"

#include <iostream>

bool NbfsServer::Initialize() {

	std::cout << __FILE__ << "--" << __FUNCTION__ << std::endl;

	return true;
}

int main(int argc, char* argv[]) {

	std::cout << "test ..." << std::endl;

	return nebula::DoMain<NbfsServer>(argc, argv);
}
