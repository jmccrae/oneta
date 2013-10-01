val in1 = io.Source.fromFile(args(0)).getLines
val in2 = io.Source.fromFile(args(1)).getLines
val topN = args(2).toInt
val corpus = io.Source.fromFile(args(3)).getLines
val corpus2 = io.Source.fromFile(args(3)).getLines

val scores = (for(((l1,l2),cl) <- ((in1 zip in2) zip corpus2)) yield {
  val i = l1.toDouble
  val o = l2.toDouble
  val l = cl.split("\\s+").size
  if(l > 0) {
    (i - o) / l
  } else {
    0.0
  }
}).toList

val thresh = scores.filter(_!=0.0).sortBy(-_).apply(topN)

System.err.println("Threshold: " + thresh)

var i = 1

for((l,s) <- (corpus zip scores.iterator)) {
  if(s >= thresh && s != 0.0) {
//    println(s + ":" + l)
    println(i)
  }
  i += 1
}
