var AddField = function() {
    var cursor = db.user.find();
    while (cursor.hasNext()) {
        var ps = cursor.next();
        ps.account = ps.deviceid + "@guest";
        db.user.save(ps);
    }
}
//AddField();