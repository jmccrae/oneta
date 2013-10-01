val scores = io.Source.fromFile(args(0)).getLines.drop(1) map {
    line => line split(",") match {
        case Array(x,_,s) => (x.toInt,s.toDouble)
    }
}

val top = scores.toList.sortBy(-_._2).take(args(1).toInt)

for((x,_) <- top) {
    println(x)
}
