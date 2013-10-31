import java.io._

val out = new PrintWriter("docs." + lang)

out.println(io.Source.fromFile(args(0)).getLines.mkString(" "))

out.flush
out.close
