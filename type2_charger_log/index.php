<?PHP session_start();
ob_start();
error_reporting(E_ERROR);
////////	/////////	///////////	 /////////
//		/	//		//		//		 //		 //
//		//	//		//		//		 //		 //
//		//	////////		//		 ////////
//		/	//				//		 //
////////	//				//		 //


//DPTP System Neo-Geo.hu 2020 / Type2 Charger Analizing
////////////////////////////////////////////////////////////
/// Programot írta és design-t tervezte: DPTP System 	 ///
/// http://www.don-peter.webege.com						 ///
/// E-mail: don_peter@freemail.hu					   	 ///
/// 2017 Copyright DPTP Minden jog fentartva			 ///
///             All right reserved.						 ///	
////////////////////////////////////////////////////////////
?>

<!doctype html>
<html lang="hu">
	<head>
		<meta http-equiv="Content-Type" content="text/html; charset=ISO-8859-2" />
		<TITLE id="html_title">DPTP System - Töltés adatok elemzése</TITLE>
		<meta http-equiv="Cache-Control" content="max-age=3600" />
		<meta name="expires" content="NEVER" />
		<meta name="revisit-after" content="7 day" />
		<link rel="stylesheet" type="text/css" href="style/style.css?version=100818" media="all" />
		<meta name="keywords" content="DPTP, dptp, dptp system, DPTP System, type2, charger" />
		<meta name="Description" content="DPTP System - Töltés adatok elemzése" />
		<meta name="robots" content="all" />
		<meta name="publisher" content="http://neo-geo.hu/type2" />
		<meta name="site_name" content="DPTP System - Töltés adatok elemzése"/>
		<meta name="GOOGLEBOT" content="ALL" />
		<meta name="rating" content="general" />
		<meta name="resource-type" content="document" />
		<meta name="robots" content="index, follow" />
		<link rel="shortcut icon" type="icon/ico" href="favicon.ico" />
		<meta name="author" content="DPTP System - Töltés adatok elemzése" />
		<meta name="reply-to" content="don_peter@freemail.hu" />
		<meta name="Author" content="ACTIVE VISION http://www.neo-geo.hu/type2" />
		<meta name="Author" content="SIWWWA" />
		<meta name="Author" content="BlondXML" />
		<meta name="publisher" content="DPTP System - Töltés adatok elemzése" />
		<meta name="copyright" content="DPTP System - Töltés adatok elemzése" />
		<link type="image/x-icon" rel="icon" href="/favicon.ico" />
		<link type="image/x-icon" rel="shortcut icon" href="/favicon.ico" />
		<meta http-equiv="language" content="hu" />
		<script type="text/javascript" src="javascript/jquery-1.7.1.js"></script>
		<script src="Chart.bundle.js"></script>
		<script src="utils.js"></script>

		<!-- html5.js for IE less than 9 -->
		<!--[if lt IE 9]>
		 <script src="javascript/html5.js"></script>
		<![endif]-->
		<!-- css3-mediaqueries.js IE less than 9 -->
		<!--[if lt IE 9]>
		 <script src="javascript/css3-mediaqueries.js"></script>
		<![endif]-->

		<style>
		canvas {
			-moz-user-select: none;
			-webkit-user-select: none;
			-ms-user-select: none;
		}
		</style>

	</head>
	<body>
</style>
		<?PHP
			
			$mappa = "log";
			$i = 0;
			$konyvtar = opendir($mappa);
			print('<form method="post" action="">');
				print('Válasz egy fájlt: ');
				print('<select name="file">');
					print('<option value="'.$_POST["file"].'" selected>'.$_POST["file"].'</option>');
					while(($fajl = readdir($konyvtar)) !== false){
						if(!is_dir("$mappa/$fajl") AND $fajl != "." AND $fajl != ".." AND $fajl != "Thumbs.db"){
							//print('<a href="index.php?file='.$mappa.'/'.$fajl.'">'.$fajl.'</a>, ');
							print('<option value="'.$mappa.'/'.$fajl.'">'.$fajl.'</option>');
						}
					}
				print('</select>');
				print('<input type="submit" value="Megnézem">');
			print('</form>');
			closedir($konyvtar);
			

		if(isset($_POST["file"])){
			$z = 0;
			$y = 0;
			$filename = $_POST["file"]; 
			$fnev = explode('_', $filename);
			$byteArray = file_get_contents($filename);
			$sorok = explode("#", $byteArray);
			for($i=0; $i<count($sorok)-1; $i++)
			{
				$adatok[$i] = explode(',', $sorok[$i]);
			}
			/*echo('<pre>');
				print_r($fnev);
			echo('</pre>');*/
			// Fájl neve
			/*
			[0] => log/2020
			[1] => 01
			[2] => 30
			[3] => 11
			[4] => 00
			[5] => toltoadatok.txt
			*/
			$ev = explode("/", $fnev[0]);
			$TimeS = strtotime($ev[1]."-".$fnev[1]."-".$fnev[2]." ".$fnev[3].":".$fnev[4])-$adatok[count($adatok)-1][0];
			// idő, Analog jel, kitőltés, áram, teljesítmény, össze teljesítmény, összes áram, feszültség
			/*
			[0] => 10		-	eltelt idő másodpercben
            [1] => 1810		-	Analog jel
            [2] => 9		-	Kitőltési tényező
            [3] => 5.52		-	Töltő áram
            [4] => 1269.60	-	Töltő teljesítmény
            [5] => 3.90		-	Összes teljesítmény
            [6] => 0.00		-	Össze áram (mA)
            [7] => 5744		-	Feszültség
			*/
			
		?>
			<div id="container" style="width: 96%;"><canvas id="canvas"></canvas></div>
	
			<script>
				var color = Chart.helpers.color;
				</script>
				<?PHP
				print("<script>");
				print("var barChartData = {
					labels: ");
					print("[");

					// Ide jönnek a pontok elnevezései, jelen pillanatban szerintem időpontok lesznek ezek
					for($i=0; $i<count($adatok)-1; $i++)
					{
						print("'".date('Y-m-d H:i:s', ($TimeS+$adatok[$i][0]))."'".(($i<count($adatok)-1)?',':''));
					}
					print("],");
					print("
					datasets: [");
					
						print("{");
							print("type: 'line',");
							print("fill: false,");
							print("label: 'Töltő áram',
							backgroundColor: color(window.chartColors.green).alpha(0.5).rgbString(),
							borderColor: window.chartColors.green,
							borderWidth: 1,
							data: [");
								// Ide jönnek a töltő áram értékek
								for($i=0; $i<count($adatok)-1; $i++)
								{
									print("'".$adatok[$i][3]."'".(($i<count($adatok)-1)?',':''));
								}
							print("]
						}");

					print(",");
					
						print("{");
							print("type: 'line',");
							print("fill: false,");
							print("label: 'Felvett áram',
							backgroundColor: color(window.chartColors.blue).alpha(0.5).rgbString(),
							borderColor: window.chartColors.blue,
							borderWidth: 1,
							data: [");
								// Ide jönnek a felvett összes teljesítmények
								for($i=0; $i<count($adatok)-1; $i++)
								{
									print("'".$adatok[$i][6]."'".(($i<count($adatok)-1)?',':''));
								}
							print("]
						}");
					
					print(",");
					
						print("{");
							print("type: 'line',");
							print("fill: false,");
							print("label: 'Teljesítmény',
							backgroundColor: color(window.chartColors.red).alpha(0.5).rgbString(),
							borderColor: window.chartColors.red,
							borderWidth: 1,
							data: [");
								// Ide jönnek a teljesítmény értékek
								for($i=0; $i<count($adatok)-1; $i++)
								{
									print("'".$adatok[$i][4]."'".(($i<count($adatok)-1)?',':''));
								}
							print("]
						}");
					
					print(",");
					
						print("{");
							print("type: 'line',");
							print("fill: false,");
							print("label: 'Felvett teljesítmény',
							backgroundColor: color(window.chartColors.orange).alpha(0.5).rgbString(),
							borderColor: window.chartColors.orange,
							borderWidth: 1,
							data: [");
								// Ide jönnek a felvett összes teljesítmények
								for($i=0; $i<count($adatok)-1; $i++)
								{
									print("'".$adatok[$i][5]."'".(($i<count($adatok)-1)?',':''));
								}
							print("]
						}");
					
					print("]

				};");
				print("</script>");
				?>
				<script>
				window.onload = function() {
					var ctx = document.getElementById("canvas").getContext("2d");
					window.myBar = new Chart(ctx, {
						type: 'bar',
						data: barChartData,
						options: {
							responsive: true,
							legend: {
								position: 'top',
							},
							title: {
								display: true,
								text: 'DPTP System - Töltés adatok elemzése'
							}
						}
					});

				};
				
			</script>
			<?PHP
		
		}
		?>

	</body>
</html>