import java.io._
import java.util.zip.GZIPInputStream

val docStartLine = ".* fromDoc=\"(.*)\" toDoc=\"(.*)\".*".r
val alignLine = ".* xtargets=\"(.*);(.*)\".*".r

var fromFile : Iterator[String] = null
var toFile : Iterator[String] = null
var fromOut : PrintWriter = null
var toOut : PrintWriter = null
val prefix = "OpenSubtitles2012/filtered/"
val out    = "OpenSubtitles2012/docs/"

new java.io.File(out).mkdirs

def changeSuffix(s : String) = s.replaceAll("\\.xml\\.gz$",".txt")

def doAlign(in : Iterator[String], out : PrintWriter, idxs : String) = {
    if(idxs == "") {
        in
    } else {
        var buf = new StringBuilder()
        var v = in
        for(l1 <- idxs.split(" ").map(_.toInt)) {
            v = v.dropWhile(!_.contains("id=\"" + l1 + "\""))
            var s = ""
            while(!s.contains("</s>")) {
                buf.append(s)
                s = v.next
            }
        }
        val line = buf.toString.replaceAll("<[^>]*>","").replaceAll("\\s+"," ").replaceAll("^\\s+","")
        out.println(line)
        v
    }
}

io.Source.stdin.getLines.foreach {
    case docStartLine(from,to) => {
        Option(fromOut).map(_.flush)
        Option(fromOut).map(_.close)
        Option(toOut).map(_.flush)
        Option(toOut).map(_.close)
        fromFile = io.Source.fromInputStream(new GZIPInputStream(new FileInputStream(prefix+from))).getLines
        toFile = io.Source.fromInputStream(new GZIPInputStream(new FileInputStream(prefix+to))).getLines
        new File(out + from).getParentFile().mkdirs
        new File(out + to).getParentFile().mkdirs
        fromOut = new PrintWriter(out + changeSuffix(from))
        toOut = new PrintWriter(out + changeSuffix(to))
    }
    case alignLine(f,t) => {
        fromFile = doAlign(fromFile,fromOut,f)
        toFile = doAlign(toFile,toOut,t)
    }
    case _ =>
}      
Option(fromOut).map(_.flush)
Option(fromOut).map(_.close)
Option(toOut).map(_.flush)
Option(toOut).map(_.close)