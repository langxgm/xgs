protoc -I . --cpp_out=../link/protos core.proto link2ls_meta.proto verify.proto login.proto
protoc -I . --cpp_out=../login/protos core.proto link2ls_meta.proto login.proto
protoc -I . --cpp_out=../gate/protos core.proto meta.proto verify.proto login2game.proto
protoc -I . --cpp_out=../world/protos core.proto meta.proto login2game.proto
protoc -I . --cpp_out=../game/protos core.proto meta.proto login2game.proto

protoc -I . --cpp_out=../game/protos gamedata.proto
protoc -I . --cpp_out=../game/protos mail.proto
protoc -I . --cpp_out=../game/protos feedback.proto
