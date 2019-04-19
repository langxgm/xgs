protoc -I . --cpp_out=../link/protos login2sdk.proto
protoc -I . --cpp_out=../login/protos login2sdk.proto
protoc -I . --cpp_out=../sdk/protos core.proto login2sdk.proto
