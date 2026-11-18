$ErrorActionPreference = 'Stop'
$OutDir = Join-Path $PSScriptRoot ''
$PptxPath = Join-Path $OutDir 'C2-Learn-Project-Presentation.pptx'
$PdfPath = Join-Path $OutDir 'C2-Learn-Project-Presentation.pdf'
$W = 960.0; $H = 540.0
$C = @{ bg='#0B1020'; panel='#151D32'; panel2='#1B2740'; white='#F4F7FC'; muted='#A8B4CA'; cyan='#32C5F4'; blue='#3B82F6'; teal='#28C6AE'; purple='#A855F7'; orange='#FFB24A'; pink='#F05D78'; green='#4CD6A4'; line='#33415E'; ink='#0B1020' }
function Rgb([string]$hex) { $v=$hex.TrimStart('#'); $r=[Convert]::ToInt32($v.Substring(0,2),16); $g=[Convert]::ToInt32($v.Substring(2,2),16); $b=[Convert]::ToInt32($v.Substring(4,2),16); return [int]($r + ($g -shl 8) + ($b -shl 16)) }
function NewSlide([string]$eyebrow,[string]$title,[int]$n) {
  $slide=$pres.Slides.Add($pres.Slides.Count+1,12); $slide.Background.Fill.Solid(); $slide.Background.Fill.ForeColor.RGB=Rgb $C.bg
  AddText $slide 44 23 700 18 $eyebrow.ToUpper() 9 $C.cyan $true 1 | Out-Null
  AddText $slide 44 48 870 42 $title 25 $C.white $true 1 | Out-Null
  AddText $slide 44 518 500 12 'C² LEARN  •  PROJECT PRESENTATION' 8 $C.muted $true 1 | Out-Null
  AddText $slide 900 516 28 14 ('{0:D2}' -f $n) 9 $C.cyan $true 3 | Out-Null
  return $slide
}
function AddText($s,[double]$x,[double]$y,[double]$w,[double]$h,[string]$text,[double]$size,[string]$color,[bool]$bold=$false,[int]$align=1) {
  $sh=$s.Shapes.AddTextbox(1,$x,$y,$w,$h); $sh.Fill.Visible=0; $sh.Line.Visible=0
  $tf=$sh.TextFrame; $tf.MarginLeft=0; $tf.MarginRight=0; $tf.MarginTop=0; $tf.MarginBottom=0; $tf.WordWrap=-1; $tf.AutoSize=0; $tf.VerticalAnchor=1
  $tr=$tf.TextRange; $tr.Text=$text; $tr.Font.Name='Aptos'; $tr.Font.Size=$size; $tr.Font.Bold=$bold; $tr.Font.Color.RGB=Rgb $color; $tr.ParagraphFormat.Alignment=$align
  return $sh
}
function AddBox($s,[double]$x,[double]$y,[double]$w,[double]$h,[string]$fill,[string]$stroke='') {
  $sh=$s.Shapes.AddShape(5,$x,$y,$w,$h); $sh.Fill.Solid(); $sh.Fill.ForeColor.RGB=Rgb $fill
  if($stroke){$sh.Line.Visible=-1;$sh.Line.ForeColor.RGB=Rgb $stroke;$sh.Line.Weight=1.2}else{$sh.Line.Visible=0}; return $sh
}
function AddCircle($s,[double]$x,[double]$y,[double]$d,[string]$fill,[string]$stroke='') {
  $sh=$s.Shapes.AddShape(9,$x,$y,$d,$d);$sh.Fill.Solid();$sh.Fill.ForeColor.RGB=Rgb $fill
  if($stroke){$sh.Line.Visible=-1;$sh.Line.ForeColor.RGB=Rgb $stroke;$sh.Line.Weight=1.2}else{$sh.Line.Visible=0};return $sh
}
function AddLine($s,[double]$x1,[double]$y1,[double]$x2,[double]$y2,[string]$color,[double]$weight=2,[bool]$arrow=$false) {
  $sh=$s.Shapes.AddLine($x1,$y1,$x2,$y2);$sh.Line.ForeColor.RGB=Rgb $color;$sh.Line.Weight=$weight;if($arrow){$sh.Line.EndArrowheadStyle=3};return $sh
}
function AddPill($s,[double]$x,[double]$y,[double]$w,[string]$label,[string]$fill,[string]$textColor=$C.white) {
  AddBox $s $x $y $w 24 $fill | Out-Null; AddText $s ($x+8) ($y+5) ($w-16) 14 $label 9 $textColor $true 2 | Out-Null
}
function AddCard($s,[double]$x,[double]$y,[double]$w,[double]$h,[string]$accent,[string]$title,[string]$body,[string]$tag='') {
  AddBox $s $x $y $w $h $C.panel $C.line | Out-Null
  AddBox $s ($x+16) ($y+17) 5 30 $accent | Out-Null
  AddText $s ($x+32) ($y+16) ($w-48) 24 $title 15 $C.white $true 1 | Out-Null
  AddText $s ($x+18) ($y+58) ($w-36) ($h-80) $body 11 $C.muted $false 1 | Out-Null
  if($tag){AddPill $s ($x+18) ($y+$h-34) 105 $tag $C.panel2 $accent | Out-Null}
}
function AddMockWindow($s,[double]$x,[double]$y,[double]$w,[double]$h,[string]$name) {
  AddBox $s $x $y $w $h '#10182A' $C.line | Out-Null
  AddBox $s $x $y $w 23 '#202B42' | Out-Null
  AddCircle $s ($x+10) ($y+8) 6 $C.pink | Out-Null; AddCircle $s ($x+21) ($y+8) 6 $C.orange | Out-Null; AddCircle $s ($x+32) ($y+8) 6 $C.green | Out-Null
  AddText $s ($x+48) ($y+5) ($w-55) 14 $name 8 $C.muted $true 1 | Out-Null
}

$pp=New-Object -ComObject PowerPoint.Application
$pp.Visible=-1
$pres=$pp.Presentations.Add()
$pres.PageSetup.SlideSize=16
$pres.PageSetup.SlideWidth=$W; $pres.PageSetup.SlideHeight=$H

# 1 — cover
$s=$pres.Slides.Add(1,12);$s.Background.Fill.Solid();$s.Background.Fill.ForeColor.RGB=Rgb $C.bg
AddPill $s 48 34 168 'C / C++ • DESKTOP APP' $C.panel2 $C.cyan | Out-Null
AddText $s 48 91 520 88 'C² Learn' 48 $C.white $true 1 | Out-Null
AddText $s 50 183 480 78 'Learn code by riding the rollercoaster.' 27 $C.cyan $true 1 | Out-Null
AddText $s 50 284 440 65 'A visual C and C++ learning experience built around lessons, practice, games, and progress.' 16 $C.muted $false 1 | Out-Null
AddText $s 50 405 410 38 'PROJECT WALKTHROUGH  •  TEAM PRESENTATION' 10 $C.orange $true 1 | Out-Null
AddText $s 50 448 500 24 'C11 + Raylib   |   Desktop   |   Offline local profiles' 11 $C.muted $false 1 | Out-Null
# cover coaster illustration
AddBox $s 566 69 346 402 $C.panel $C.line | Out-Null
AddText $s 592 92 290 20 'A LEARNING PATH WITH MOMENTUM' 9 $C.cyan $true 1 | Out-Null
$pts=@(@(602,345),@(665,345),@(704,278),@(752,278),@(785,365),@(824,365),@(866,230))
for($i=0;$i -lt $pts.Count-1;$i++){AddLine $s $pts[$i][0] $pts[$i][1] $pts[$i+1][0] $pts[$i+1][1] $C.cyan 3 $false|Out-Null}
$nodeColors=@($C.blue,$C.teal,$C.orange,$C.purple,$C.green,$C.pink,$C.cyan)
for($i=0;$i -lt $pts.Count;$i++){AddCircle $s ($pts[$i][0]-9) ($pts[$i][1]-9) 18 $nodeColors[$i] $C.bg|Out-Null}
AddPill $s 597 391 98 'LEARN' $C.panel2 $C.white|Out-Null; AddPill $s 707 391 98 'PRACTISE' $C.panel2 $C.white|Out-Null; AddPill $s 817 391 72 'GROW' $C.panel2 $C.white|Out-Null
AddText $s 594 430 300 20 'Small steps. Visible progress. More confidence.' 11 $C.muted $false 2|Out-Null
AddText $s 48 518 500 12 'C² LEARN  •  PROJECT PRESENTATION' 8 $C.muted $true 1|Out-Null
AddText $s 900 516 28 14 '01' 9 $C.cyan $true 3|Out-Null

# 2 — project pitch
$s=NewSlide '01 / The idea' 'Turn learning into an experience you can act on' 2
AddText $s 44 101 872 30 'C² Learn brings C/C++ concepts into one desktop place where learners can read, try, see, and repeat.' 15 $C.muted $false 1|Out-Null
AddCard $s 44 162 276 214 $C.orange 'LEARN' 'Move through a rollercoaster-style map of topics, lessons, and challenges.' 'Guided progression helps turn a large syllabus into smaller milestones.' 'GUIDED PATH'
AddCard $s 342 162 276 214 $C.cyan 'PRACTISE' 'Answer, debug, and write code with quick feedback.' 'Multiple question formats and coding activities make learners participate instead of only reading.' 'HANDS-ON'
AddCard $s 640 162 276 214 $C.green 'SEE PROGRESS' 'Keep local profiles, XP, streaks, and badges.' 'Progress gives the learner a reason to return and a simple way to see what they have completed.' 'MOTIVATION'
AddBox $s 44 405 872 76 $C.panel2 | Out-Null
AddText $s 65 420 180 16 'THE PROJECT IN ONE LINE' 9 $C.cyan $true 1|Out-Null
AddText $s 250 415 630 42 'A playful learning hub that combines curriculum, coding practice, visual explainers, and game-based challenges.' 16 $C.white $true 1|Out-Null

# 3 — feature tour
$s=NewSlide '02 / What it does' 'Six ways into the learning journey' 3
$cards=@(
 @('01','Guided learning','Topics, lessons, and rollercoaster progression.',$C.orange),
 @('02','Code practice','Multiple choice, fill-in, bug spotting, and short code tasks.',$C.cyan),
 @('03','Explore','DSA visualisers, system design, CS, and AI topics.',$C.teal),
 @('04','Study tools','Reference content and focused study features.',$C.purple),
 @('05','Games','Puzzle activities plus 2D and 3D chess against an AI.',$C.pink),
 @('06','Progress and access','Profiles, XP, streaks, badges, and accessibility settings.',$C.green)
)
for($i=0;$i -lt 6;$i++){ $col=$i%3;$row=[math]::Floor($i/3);$x=44+$col*298;$y=117+$row*170; $item=$cards[$i]; AddBox $s $x $y 276 148 $C.panel $C.line|Out-Null; AddCircle $s ($x+18) ($y+18) 34 $item[3]|Out-Null; AddText $s ($x+18) ($y+27) 34 15 $item[0] 9 $C.ink $true 2|Out-Null; AddText $s ($x+64) ($y+22) 195 22 $item[1] 15 $C.white $true 1|Out-Null; AddText $s ($x+20) ($y+70) 236 55 $item[2] 11 $C.muted $false 1|Out-Null }
AddText $s 44 469 860 20 'The strongest presentation demo: jump from Explore to the step-by-step video streaming architecture.' 11 $C.cyan $true 1|Out-Null

# 4 — screen previews, editable recreations
$s=NewSlide '03 / Interface tour' 'A quick look at the experience' 4
AddText $s 44 97 850 20 'Editable UI previews reconstructed from the project screens; they are not runtime screenshots.' 10 $C.orange $false 1|Out-Null
# preview 1: menu
AddMockWindow $s 44 135 276 250 'HOME MENU'
AddText $s 63 169 230 22 'C² Learn' 20 $C.white $true 1|Out-Null
AddText $s 63 193 230 15 'Learn C/C++ by riding the rollercoaster' 8 $C.muted $false 1|Out-Null
$labels=@('Continue learning','Code practice','Explore','Puzzle games')
for($i=0;$i -lt 4;$i++){AddBox $s 63 (226+$i*32) 238 25 $(if($i -eq 0){$C.blue}else{$C.panel2})|Out-Null;AddText $s 73 (232+$i*32) 214 14 $labels[$i] 9 $C.white $true 1|Out-Null}
AddText $s 63 362 235 16 'Pick a destination from one clear hub.' 8 $C.muted $false 1|Out-Null
# preview 2 Explore
AddMockWindow $s 342 135 276 250 'EXPLORE'
AddText $s 360 169 235 21 'Explore' 17 $C.white $true 1|Out-Null
$cats=@(@('Data structures',$C.orange),@('Computer science',$C.blue),@('System design',$C.teal),@('AI concepts',$C.pink))
for($i=0;$i -lt 4;$i++){ $x=360+($i%2)*116;$y=204+[math]::Floor($i/2)*72;AddBox $s $x $y 108 60 $C.panel2 $C.line|Out-Null;AddCircle $s ($x+9) ($y+10) 12 $cats[$i][1]|Out-Null;AddText $s ($x+9) ($y+31) 94 21 $cats[$i][0] 8 $C.white $true 1|Out-Null }
AddText $s 360 354 235 25 'Open a topic, then step through a visualiser.' 8 $C.muted $false 1|Out-Null
# preview 3 chess
AddMockWindow $s 640 135 276 250 'CHESS ARENA'
AddText $s 659 169 230 21 'Chess vs AI' 16 $C.white $true 1|Out-Null
$light='#DDE6F2';$dark='#41516D';$piece=$C.white;$piece2=$C.ink
for($r=0;$r -lt 4;$r++){for($col=0;$col -lt 4;$col++){AddBox $s (660+$col*33) (205+$r*33) 33 33 $(if((($r+$col)%2)-eq 0){$light}else{$dark})|Out-Null}}
# simplified piece silhouettes as native circle/triangle shapes
for($col=0;$col -lt 4;$col++){AddCircle $s (669+$col*33) 214 14 $piece2|Out-Null;AddCircle $s (669+$col*33) 313 14 $piece|Out-Null}
AddBox $s 808 210 91 105 '#1B2740' '#33415E'|Out-Null;AddText $s 817 224 74 70 '2D / 3D chess' 9 $C.white $true 1|Out-Null
AddText $s 659 354 230 20 'Select a piece. Review highlighted moves.' 8 $C.muted $false 1|Out-Null
AddText $s 44 414 870 52 'For final delivery, replace these editable previews with fresh in-app captures if you want literal screenshots. The rest of this deck is built from editable shapes and text.' 11 $C.muted $false 1|Out-Null

# 5 — architecture walkthrough
$s=NewSlide '04 / Featured visualiser' 'Video streaming, explained one step at a time' 5
AddText $s 44 96 865 18 'The Explore system design diagram turns a service architecture into a guided, clickable flow.' 11 $C.muted $false 1|Out-Null
AddBox $s 44 132 586 320 '#111A2D' $C.line|Out-Null
AddText $s 62 148 420 18 'VIDEO STREAMING PIPELINE' 10 $C.cyan $true 1|Out-Null
# flow nodes positions
$nodes=@(
 @('PLAYER',90,221,$C.blue),@('EDGE CDN',250,221,$C.teal),@('ORIGIN LB',420,221,$C.purple),@('OBJECT STORE',420,331,$C.orange),@('TRANSCODER',250,331,$C.pink),@('CATALOG DB',90,331,$C.green)
)
foreach($n in $nodes){AddBox $s $n[1] $n[2] 112 42 $C.panel2 $n[3]|Out-Null;AddText $s ($n[1]+7) ($n[2]+13) 98 15 $n[0] 9 $C.white $true 2|Out-Null}
AddLine $s 202 242 250 242 $C.cyan 2 $true|Out-Null;AddLine $s 362 242 420 242 $C.cyan 2 $true|Out-Null;AddLine $s 476 263 476 331 $C.orange 2 $true|Out-Null;AddLine $s 420 352 362 352 $C.pink 2 $true|Out-Null;AddLine $s 250 352 202 352 $C.green 2 $true|Out-Null
AddText $s 64 398 540 24 'Player → CDN → origin services → cached stream back to the viewer' 9 $C.muted $false 1|Out-Null
AddText $s 662 138 250 18 'THE WALKTHROUGH' 10 $C.cyan $true 1|Out-Null
$steps=@(
 @('01','Request manifest','Player asks the nearest edge for a stream.',$C.blue),
 @('02','Cache miss','The CDN forwards the request to the origin.',$C.teal),
 @('03','Fetch and prepare','Storage and transcode services supply segments.',$C.orange),
 @('04','Cache and deliver','The edge serves chunks to viewers.',$C.green)
)
for($i=0;$i -lt 4;$i++){ $y=170+$i*66;AddCircle $s 664 $y 30 $steps[$i][3]|Out-Null;AddText $s 664 ($y+9) 30 12 $steps[$i][0] 8 $C.ink $true 2|Out-Null;AddText $s 706 ($y+1) 206 18 $steps[$i][1] 11 $C.white $true 1|Out-Null;AddText $s 706 ($y+22) 206 34 $steps[$i][2] 9 $C.muted $false 1|Out-Null }
AddPill $s 662 449 245 'PLAY  •  STEP  •  RESET  •  INSPECT' $C.panel2 $C.cyan|Out-Null

# 6 — app flow
$s=NewSlide '05 / Project flow' 'From opening the app to seeing progress' 6
AddText $s 44 97 870 20 'The main loop connects a learners choices to feedback and locally saved progress.' 11 $C.muted $false 1|Out-Null
# top row start
$flow=@(@('START','Loading screen',$C.blue),@('CHOOSE','Menu and profile',$C.cyan),@('LEARN','Pick a module',$C.purple),@('ACT','Answer / code / explore',$C.orange),@('GROW','Feedback and progress',$C.green))
for($i=0;$i -lt 5;$i++){ $x=48+$i*181;AddBox $s $x 157 150 88 $C.panel $flow[$i][2]|Out-Null;AddPill $s ($x+14) 171 82 $flow[$i][0] $C.panel2 $flow[$i][2]|Out-Null;AddText $s ($x+12) 206 126 27 $flow[$i][1] 10 $C.white $true 2|Out-Null;if($i -lt 4){AddLine $s ($x+151) 201 ($x+178) 201 $C.muted 1.7 $true|Out-Null}}
AddText $s 70 287 810 20 'Choose a path from the menu' 10 $C.muted $true 2|Out-Null
$branches=@(@('MAP and LESSONS','Concepts → checkpoint',$C.orange),@('CODE PRACTICE','Prompt → attempt → feedback',$C.cyan),@('EXPLORE','Topic → visualiser → insight',$C.teal),@('PUZZLES','Game → strategy → result',$C.pink))
for($i=0;$i -lt 4;$i++){ $x=45+$i*222;AddBox $s $x 321 204 73 $C.panel2 $C.line|Out-Null;AddText $s ($x+12) 335 180 16 $branches[$i][0] 9 $branches[$i][2] $true 2|Out-Null;AddText $s ($x+10) 361 184 25 $branches[$i][1] 9 $C.white $false 2|Out-Null;AddLine $s 480 245 ($x+102) 321 $C.line 1 $false|Out-Null}
AddBox $s 257 431 447 48 '#102C38' $C.teal|Out-Null
AddText $s 275 445 411 19 'XP • badges • streaks • account progress saved locally' 11 $C.white $true 2|Out-Null
AddText $s 44 489 872 16 'Storage: local account CSV (accounts.csv) — no online leaderboard service is implemented.' 9 $C.muted $false 1|Out-Null

# 7 — rules and demo playbook
$s=NewSlide '06 / How to explain the rules' 'A simple loop for learning — and for the chess demo' 7
AddBox $s 44 124 423 316 $C.panel $C.line|Out-Null
AddText $s 66 147 370 24 'LEARNING LOOP' 12 $C.cyan $true 1|Out-Null
$lr=@('Create or select a local profile.','Choose a lesson, practice task, visualiser, or game.','Try the activity; use feedback or step controls to learn.','Earn progress through the app and revisit your next goal.')
for($i=0;$i -lt 4;$i++){ $y=194+$i*53;AddCircle $s 68 $y 27 $C.panel2 $C.cyan|Out-Null;AddText $s 68 ($y+7) 27 12 ([string]($i+1)) 9 $C.cyan $true 2|Out-Null;AddText $s 109 ($y-1) 325 38 $lr[$i] 11 $C.white $false 1|Out-Null }
AddBox $s 493 124 423 316 $C.panel $C.line|Out-Null
AddText $s 515 147 370 24 'CHESS ARENA DEMO' 12 $C.orange $true 1|Out-Null
$cr=@('You play White; the AI plays Black.','Click a piece, then click a highlighted destination.','Switch between 2D and 3D; in 3D, click the board and use ← / → to turn the view.','This is a simplified chess demo: pawn promotion is automatic; capturing the king ends the game.')
for($i=0;$i -lt 4;$i++){ $y=194+$i*53;AddCircle $s 517 $y 27 $C.panel2 $C.orange|Out-Null;AddText $s 517 ($y+7) 27 12 ([string]($i+1)) 9 $C.orange $true 2|Out-Null;AddText $s 558 ($y-1) 325 45 $cr[$i] 10 $C.white $false 1|Out-Null }
AddText $s 52 461 850 33 'Talk track: Pick a destination, take one action, get feedback, then see what to do next.' 15 $C.white $true 2|Out-Null

# 8 — architecture
$s=NewSlide '07 / Under the hood' 'Small modules, one connected application' 8
AddText $s 44 97 865 20 'The project is written in C11, uses Raylib for the desktop interface, and uses CMake to build.' 11 $C.muted $false 1|Out-Null
AddBox $s 48 151 205 76 $C.panel $C.blue|Out-Null;AddText $s 65 168 170 20 'main.c' 15 $C.white $true 2|Out-Null;AddText $s 65 194 170 17 'Window + render loop' 9 $C.muted $false 2|Out-Null
AddLine $s 253 189 305 189 $C.cyan 2 $true|Out-Null
AddBox $s 305 151 205 76 $C.panel $C.cyan|Out-Null;AddText $s 322 168 170 20 'app.c' 15 $C.white $true 2|Out-Null;AddText $s 322 194 170 17 'Screen routing + state' 9 $C.muted $false 2|Out-Null
AddLine $s 510 189 562 189 $C.cyan 2 $true|Out-Null
AddBox $s 562 151 345 76 $C.panel $C.purple|Out-Null;AddText $s 580 168 305 20 'Screen modules' 15 $C.white $true 2|Out-Null;AddText $s 580 194 305 17 'Learn • Practice • Explore • Games • Study' 9 $C.muted $false 2|Out-Null
AddLine $s 408 227 408 287 $C.teal 2 $true|Out-Null
AddBox $s 228 287 360 78 '#102C38' $C.teal|Out-Null;AddText $s 250 302 315 20 'Core logic and content' 13 $C.white $true 2|Out-Null;AddText $s 250 328 315 18 'Accounts • questions • levels • progress' 9 $C.muted $false 2|Out-Null
AddLine $s 408 365 408 413 $C.orange 2 $true|Out-Null
AddBox $s 228 413 360 55 $C.panel2 $C.orange|Out-Null;AddText $s 250 431 315 17 'Local data: accounts.csv' 11 $C.white $true 2|Out-Null
AddBox $s 627 287 280 181 $C.panel $C.line|Out-Null
AddText $s 649 308 235 18 'PRESENTATION-TIME DEMO PATH' 9 $C.cyan $true 1|Out-Null
AddText $s 649 339 235 96 '1  Open Explore`n2  Enter System Design`n3  Play / step the video flow`n4  Switch to Chess 3D and move' 11 $C.white $false 1|Out-Null

# 9 — licensing
$s=NewSlide '08 / License and reuse' 'Be clear about what is — and isnt — licensed' 9
AddBox $s 44 126 872 126 '#2A1A23' $C.pink|Out-Null
AddPill $s 65 147 180 'PROJECT LICENSE' $C.pink $C.white|Out-Null
AddText $s 65 184 806 48 'No top-level LICENSE file is present in this repository. The projects own reuse terms are therefore not specified here.' 15 $C.white $true 1|Out-Null
AddText $s 46 275 280 18 'DEPENDENCIES INCLUDED' 10 $C.cyan $true 1|Out-Null
AddCard $s 44 307 410 125 $C.cyan 'Raylib' 'The checked-in build fetches Raylib 6.2. Raylib uses the zlib/libpng license; preserve its notice when redistributing.' 'THIRD-PARTY'
AddCard $s 488 307 428 125 $C.orange 'Bundled fonts' 'Poppins and IBM Plex Mono font notices are included in assets/fonts; both notices specify SIL Open Font License 1.1.' 'FONT LICENSES'
AddText $s 47 456 865 35 'Before public release: agree on a project license with all contributors, add a LICENSE file, and retain dependency/font notices.' 11 $C.muted $true 1|Out-Null
AddText $s 47 494 870 14 'Sources: github.com/raysan5/raylib/blob/master/LICENSE  •  assets/fonts/OFL-Poppins.txt  •  assets/fonts/OFL-IBMPlexMono.txt' 8 $C.muted $false 1|Out-Null

# 10 — run of show / close
$s=NewSlide '09 / Present it effortlessly' 'A 90-second walkthrough for two presenters' 10
$talk=@(
 @('00:00','Set the scene','C² Learn makes programming practice feel like a journey.',$C.cyan),
 @('00:15','Show the menu','Point out the paths: learning, practice, Explore, and games.',$C.orange),
 @('00:35','Show the visualiser','Open System Design → Interactive Architecture; step through the CDN flow.',$C.teal),
 @('01:00','Show the game','Open Chess Arena; make one move in 2D, switch to 3D.',$C.purple),
 @('01:20','Close with facts','Mention local progress, C11/Raylib, and the projects current license status.',$C.green)
)
for($i=0;$i -lt 5;$i++){ $y=132+$i*65;AddBox $s 48 $y 864 53 $C.panel $C.line|Out-Null;AddText $s 66 ($y+17) 82 18 $talk[$i][0] 10 $talk[$i][3] $true 1|Out-Null;AddText $s 158 ($y+10) 174 18 $talk[$i][1] 12 $C.white $true 1|Out-Null;AddText $s 338 ($y+10) 550 34 $talk[$i][2] 10 $C.muted $false 1|Out-Null}
AddText $s 48 472 860 25 'C² Learn  •  Learn C/C++ by riding the rollercoaster.' 17 $C.cyan $true 2|Out-Null

$pres.SaveAs($PptxPath,24)
$pres.SaveAs($PdfPath,32)
$pres.Close();$pp.Quit()

