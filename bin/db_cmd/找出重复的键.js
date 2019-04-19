db.getCollection('user').aggregate({"$group":{"_id":"$deviceid","count":{"$sum":1}}},
    {"$sort":{"count":-1}},
    {"$match":{"count":{"$gte":1}}}
)