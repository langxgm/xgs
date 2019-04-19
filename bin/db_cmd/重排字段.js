var ModifyFieldOrder = function() {
    var cursor = db.user.find();
    while (cursor.hasNext()) {
        var ps = cursor.next();
        db.user.replaceOne({_id:ps._id}, {guid:ps.guid, account:ps.account, register_time:ps.register_time, login_time:ps.login_time, login_count:NumberInt(ps.login_count), deviceid:ps.deviceid, ip:ps.ip});
    }
}
//ModifyFieldOrder()