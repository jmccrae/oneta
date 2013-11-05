import java.io._

val lang = args(0)

val N = new File("OpenSubtitles2012/dedup/").listFiles.filter(_.endsWith("."+lang+".txt").size

val r = (1 to N) sortBy (x => math.random)

r.foreach(println(_))
