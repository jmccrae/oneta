import java.io.File
import java.nio.file.Files

val docLine = ".* fromDoc=\"(.*)\" toDoc=\"(.*)\".*".r

new File("OpenSubtitles2012/filtered").mkdir

val rawLoc = "OpenSubtitles2012/raw/"
val fltLoc = "OpenSubtitles2012/filtered/"

io.Source.stdin.getLines.foreach {
    case docLine(from,to) => {
        System.err.println(from + " // " + to)
        new File(fltLoc + from).getParentFile().mkdirs
        new File(fltLoc + to).getParentFile().mkdirs
        Files.copy(new File(rawLoc + from).toPath(),
                   new File(fltLoc + from).toPath())
        Files.copy(new File(rawLoc + to).toPath(),
                   new File(fltLoc + to).toPath())
    }
    case _ => 
}   
