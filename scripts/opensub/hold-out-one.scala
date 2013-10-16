import java.io._

val prefix = args(0)
val srcLang = args(1)
val trgLang = args(2)

val srcFile = new File(prefix + "." + srcLang)
val trgFile = new File(prefix + "." + trgLang)
val srcTrain = new PrintWriter(prefix + ".train." + srcLang)
val trgTrain = new PrintWriter(prefix + ".train." + trgLang)
val srcTest = new PrintWriter(prefix + ".test." + srcLang)
val trgTest = new PrintWriter(prefix + ".test." + trgLang)
val holdOut = new PrintWriter(prefix + ".heldout")

if(!srcFile.exists || !trgFile.exists) {
  System.err.println("Invalid args: " + srcFile.getPath() + " " + trgFile.getPath())
  System.exit(-1)
}

val nLines = io.Source.fromFile(srcFile).getLines.size

val heldOut = (nLines * math.random).toInt

holdOut.println(heldOut)
holdOut.flush
holdOut.close

for(((l1,l2),i) <- (io.Source.fromFile(srcFile).getLines zip io.Source.fromFile(trgFile).getLines).zipWithIndex) {
  if(i == heldOut) {
    srcTest.println(l1)
    trgTest.println(l2)
  } else {
    srcTrain.println(l1)
    trgTrain.println(l2)
  }
}
srcTrain.flush
trgTrain.flush
srcTest.flush
trgTest.flush
srcTrain.close
trgTrain.close
srcTest.close
trgTest.close

