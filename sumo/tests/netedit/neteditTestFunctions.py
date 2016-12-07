# Import libraries
import os
import sys
import subprocess
import platform
from sikuli import *

Settings.MoveMouseDelay = 0.1
Settings.DelayBeforeDrop = 0.1
Settings.DelayAfterDrag = 0.1

neteditApp = os.environ.get("NETEDIT_BINARY", "netedit")
textTestSandBox = os.environ.get("TEXTTEST_SANDBOX", ".")
currentOS = platform.system()
images = {}

def setup(neteditTests):
    # Open current environment file to obtain path to the netedit app, textTestSandBox
    envFile = os.path.join(neteditTests, "currentEnvironment.tmp")
    if os.path.exists(envFile):
        global neteditApp, textTestSandBox, currentOS
        neteditApp, textTestSandBox, currentOS = [l.strip() for l in open(envFile).readlines()]
        if not os.path.exists(textTestSandBox):
            textTestSandBox = "."
    # get reference for match
    images.update({"reference": os.path.join(neteditTests, "imageResources", "reference.png"),
                   "edit-undo": os.path.join(neteditTests, "imageResources", currentOS, "edit-undo.png"),
                   "edit-redo": os.path.join(neteditTests, "imageResources", currentOS, "edit-redo.png"),
                   "question":  os.path.join(neteditTests, "imageResources", currentOS, "question.png")})


def Popen(newNet):
    # set the default parameters of netedit
    neteditParameters = [neteditApp, '--gui-testing', '--window-size', '700,500']
    
    # check if a new net must be created, or a existent net must be loaded
    if os.path.exists(os.path.join(textTestSandBox, "input_net.net.xml")):
        neteditParameters += ['--sumo-net-file', os.path.join(textTestSandBox, "input_net.net.xml")]
    elif newNet:
        neteditParameters += ['--new', '--output-file', 'net.net.xml']
    
    # Check if additionals must be loaded
    if os.path.exists(os.path.join(textTestSandBox, "input_additionals.add.xml")):
        neteditParameters += ['--sumo-additionals-file', os.path.join(textTestSandBox, "input_additionals.add.xml")]
    else :
        neteditParameters.append('--additionals-output')
        neteditParameters.append(textTestSandBox + "/additionals.xml")
    
    return subprocess.Popen(neteditParameters, env=os.environ, stdout=sys.stdout, stderr=sys.stderr)


# obtain match 
def getMatch(neProcess) :
    try:
        print images["reference"]
        return wait(images["reference"], 20)
    except:
        neProcess.kill()
        sys.exit("Killed netedit process. 'reference.png' not found")


def setupAndStart(testRoot, newNet):
    setup(testRoot)
    # Open netedit
    neteditProcess = Popen(newNet)
    # Wait for netedit reference
    return neteditProcess, getMatch(neteditProcess)


# netedit undo
def undo(match, number) :
    click(match) #bug mit ctrl+z founded
    for x in range(0, number) :
        type("z", Key.CTRL)
    
# netedit redo
def redo(match, number) :
    click(match)
    #bug mit ctrl+y founded
    for x in range(0, number) :
        type("y", Key.CTRL)

        
# netedit modify attribute
def modifyAttribute(parametersReference, attributeNumber, value) :
    click(parametersReference)
    for x in range(0, attributeNumber) :
        type(Key.TAB)
    # select all values
    type("a", Key.CTRL)
    # paste the new value
    paste(value)
    # type enter to save change
    type(Key.ENTER)
    
# netedit modify attribute
def modifyStoppingPlaceReference(parametersReference, numTabs, numDowns) :
    click(parametersReference)
    # place cursor in comboBox Reference
    for x in range(0, numTabs) :
        type(Key.TAB)
    # Set comboBox in the first element
    for x in range(0, 3) :
        type(Key.UP)
    # select new reference
    for x in range(0, numDowns) :
        type(Key.DOWN)
    
# block additional
def changeBlockAdditional(numTabs) :
    click(parametersReference)
    # place cursor in block movement checkbox
    for x in range(0, numTabs) :
        type(Key.TAB)
    # Change current value
    type(Key.SPACE)

# netedit wait question
def waitQuestion(answer) :
    # wait 0.5 second to question dialog
    wait(0.5)
    #Answer can be "y" or "n"
    type(answer, Key.ALT)
    
# netedit quit
def quit(neteditProcess, mustBeSaved, save) :
    # quit
    type("q", Key.CTRL)

    # Check if net must be saved
    if mustBeSaved:
        if save:
            waitQuestion("y")
        else :
            waitQuestion("n")
            
    # wait 1 second
    wait(0.5)
    if neteditProcess.poll() is not None:
        print ("netedit was closed sucesfully")
    else:
        neteditProcess.kill()
        print ("error closing netedit")
    

# netedit save additionals
def saveAdditionals(match, neteditLoadedAtStart = False) :
    # first move cursor to ensure no menu item is highlighted
    hover(match.getTarget().offset(0, -120))
    
    # open file menu
    type("f", Key.ALT)
    
    # select option "save additionals" depending of neteditLoadedAtStart
    if neteditLoadedAtStart:
        for x in range(0, 13):
            type(Key.DOWN)
    else:
        for x in range(0, 12):
            type(Key.DOWN)
    type(Key.ENTER)