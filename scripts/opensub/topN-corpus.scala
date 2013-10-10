val prefix = args(0)
val suffix = args(1)
val topN = io.Source.fromFile(args(2)).getLines.map(_.toInt)

for(n <- topN) {
  io.Source.fromFile(prefix + n + suffix).getLines foreach (println(_))
}
