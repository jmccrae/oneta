import java.io._

val testN = args(0).toInt
val topNFile = io.Source.fromFile(args(1))
val lang1 = args(2)
val lang2 = args(3)
val out1 = new PrintWriter("corpus-"+lang1+"-"+lang2+"."+lang1)
val out2 = new PrintWriter("corpus-"+lang1+"-"+lang2+"."+lang2)

val topN = topNFile.getLines.map(_.toInt).toSet

var i = 0

def realI(j : Int) = if(j < testN) { 
  j+1
} else {
  j
}

for(file <- new File("OpenSubtitles2012/dedup/").listFiles.sortBy(_.getName())) {
  if(file.getPath() endsWith (lang1 + ".txt")) {
    if(topN contains realI(i)) {
      io.Source.fromFile(file.getPath()).getLines.foreach(out1.println(_))
      val p2 = file.getPath().replaceAll(lang1 + "\\.txt$",lang2 + ".txt")
      io.Source.fromFile(p2).getLines.foreach(out2.println(_))
    }
    i += 1
  }
}
out1.flush
out2.flush
out1.close
out2.close
