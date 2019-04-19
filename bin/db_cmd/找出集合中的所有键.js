map = function() {
    for (var key in this) {
        emit(key, {count:1});
    }
}
reduce = function(key, emits) {
    total = 0;
    for (var i in emits) {
        total += emits[i].count;
    }
    return {count : total};
}
//mr = db.runCommand({"mapReduce" : "user", "map" : map, "reduce" : reduce, "out" : {inline : 1} })