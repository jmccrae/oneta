if(args.length != 7 && args.length != 6) {
  System.err.println("Usage: scala supersampling.scala corpus1 corpus2 scores out1 out2 rate [boost]")
  System.exit(-1)
}

val rate = args(5).toDouble
val boost = args.length match {
  case 7 => args(6).toDouble
  case _ => 0.0
}
val corpus1 = io.Source.fromFile(args(0)).getLines
val corpus2 = io.Source.fromFile(args(1)).getLines
val scores = io.Source.fromFile(args(2)).getLines
val out1 = new java.io.PrintWriter(args(3))
val out2 = new java.io.PrintWriter(args(4))

def rpoisson(lambda : Double) : Int = {
  val L = math.exp(-lambda)
  var k = 0
  var p = 1.0
  do {
    k = k + 1
    p *= math.random
  } while(p > L)
  return k - 1
}

for(((line1,line2),scores) <- (corpus1 zip corpus2 zip scores)) {
  val score = (scores split (","))(2).toDouble
  val lambda = math.max(0,score*rate + boost)
  for(i <- 0 until rpoisson(lambda)) {
    out1.println(line1)
    out2.println(line2)
  }
}
