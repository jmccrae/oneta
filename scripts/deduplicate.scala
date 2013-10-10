import java.io.File

val lang1 = args(0)
val lang2 = args(1)
val tolerance = args(2).toDouble

type Spectrum = Map[String,Int]

def spectrum(s : String) = s.split("\\s+").groupBy(x=>x).map {
  case (x,y) => (x,y.size)
} filter {
  case (x,y) => y > 5
}

def cosine(x : Spectrum, y : Spectrum) = {
  val ab = ((x.keys.toSet & y.keys.toSet).toList map { 
    k => x(k) * y(k)
  }).sum
  val aa = (x.values map {
    a => a * a
  }).sum
  val bb = (y.values map {
    b => b * b
  }).sum

  ab.toDouble / math.sqrt(aa.toDouble) / math.sqrt(bb.toDouble)
}

val specs1 = collection.mutable.ListBuffer[Spectrum]()
val specs2 = collection.mutable.ListBuffer[Spectrum]()

var accept = 0

val dataDirPath = "OpenSubtitles2012/docs/"
val outPath = "OpenSubtitles2012/dedup/"
new File(outPath).mkdirs()
val dataDir = new File(dataDirPath)

val files1 = dataDir.listFiles.filter(_.getName().endsWith("."+lang1 + ".txt"))
val files2 = dataDir.listFiles.filter(_.getName().endsWith("."+lang2 + ".txt"))

if(files1.size != files2.size) {
  System.err.println("File sets differ in length")
  System.exit(-1)
}

val N = files1.size

implicit def string2path(s : String) = new File(s).toPath()

for(i <- 0 until N) {
  val l1 = ("" /: io.Source.fromFile(dataDirPath + "doc" + i + "." + lang1 + ".txt").getLines) (_+_)
  val l2 = ("" /: io.Source.fromFile(dataDirPath + "doc" + i + "." + lang2 + ".txt").getLines) (_+_)
  val s1 = spectrum(l1)
  val s2 = spectrum(l2)

  val sim1 = specs1 map (s => cosine(s,s1))
  val sim2 = specs2 map (s => cosine(s,s2))
  if(!(sim1 exists (_ > tolerance)) && !(sim2 exists (_ > tolerance))) {
    java.nio.file.Files.copy(dataDirPath + "doc" + i + "." + lang1 + ".txt",
      outPath + "doc" + accept + "." + lang1 + ".txt")
    java.nio.file.Files.copy(dataDirPath + "doc" + i + "." + lang2 + ".txt",
      outPath + "doc" + accept + "." + lang2 + ".txt")
    System.err.print("+")
    specs1.append(s1)
    specs2.append(s2)
    accept += 1
  } else {
    System.err.print("-")
  }
}
System.err.println()
