import java.io.File

val docLine = ".* fromDoc=\"(.*)\" toDoc=\"(.*)\".*".r

new File("OpenSubtitles2012/filtered").mkdir

val rawLoc = "OpenSubtitles2012/raw/"
val fltLoc = "OpenSubtitles2012/filtered/"

def cp(f1 : String, f2 : String) = Runtime.getRuntime().exec("cp %s %s" format(f1,f2)).waitFor()

io.Source.stdin.getLines.foreach {
    case docLine(from,to) => {
        System.err.println(from + " // " + to)
        new File(fltLoc + from).getParentFile().mkdirs
        new File(fltLoc + to).getParentFile().mkdirs
        cp(new File(rawLoc + from).toPath(),
                   new File(fltLoc + from).toPath())
        cp(new File(rawLoc + to).toPath(),
                   new File(fltLoc + to).toPath())
    }
    case _ => 
}   
