import java.io._

val lang = args(0)

val out = new PrintWriter("docs." + lang)

for(file <- new File("OpenSubtitles2012/docs/").listFiles.sortBy(_.getName())) {
  if (file.getPath() endsWith (lang + ".txt")) {
      out.println(io.Source.fromFile(file).getLines.mkString(" "))
  }
}
out.flush
out.close
