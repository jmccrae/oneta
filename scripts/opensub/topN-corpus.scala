val prefix = args(0)
val suffix = args(1)
val dedups = io.Source.fromFile(args(2)).getLines.map {
  line => line split "," match {
    case Array(x,y) => (y.toInt,x.toInt)
  }
} toMap
val topN = io.Source.fromFile(args(3)).getLines.map(_.toInt)

for(n <- topN) {
  io.Source.fromFile(prefix + dedups(n) + suffix).getLines foreach (println(_))
}
