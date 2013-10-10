import java.io._

val lang = args(0)

val out = new PrintWriter("docs." + lang)

val N = new java.io.File("OpenSubtitles2012/dedup/").listFiles.filter(_.getName().endsWith("." + lang + ".txt")).size

//for(file <- new File("OpenSubtitles2012/dedup/").listFiles.sortBy(_.getName())) {
for(i <- 0 until N) {
  val file = "OpenSubtitles2012/dedup/doc" + i + "." + lang + ".txt"
  out.println(io.Source.fromFile(file).getLines.mkString(" "))
}
out.flush
out.close
