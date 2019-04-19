protoc -I . --cpp_out=../../link/pbconfig link.conf.proto
protoc -I . --cpp_out=../../login/pbconfig login.conf.proto
protoc -I . --cpp_out=../../gate/pbconfig gate.conf.proto serverinfo.conf.proto
protoc -I . --cpp_out=../../game/pbconfig game.conf.proto
protoc -I . --cpp_out=../../world/pbconfig world.conf.proto
protoc -I . --cpp_out=../../sdk/pbconfig sdk.conf.proto serverinfo.conf.proto wxappinfo.conf.proto
