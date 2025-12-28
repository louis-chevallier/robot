import cadquery
import cadquery as cq
from cadquery.vis import show
from utillc import *


servo = cq.importers.importStep('m90s_nu.step')
w = cadquery.Workplane().sphere(1).split(keepBottom=True) - cadquery.Workplane().sphere(0.5)


EKOX( dir(servo.faces('>X')))
EKOX( servo.faces('>X').size())
w = servo.faces('>X[-2]').sphere(3) 


r = w.faces('<X').fillet(0.1)
r = w
r.export("box.step")

#r = cadquery.Workplane('XY').box(1,2,3)

# Show the result
#show(r, alpha=0.5)


#cadquery.Workplane('XY').box(1,2,3).toStep()
