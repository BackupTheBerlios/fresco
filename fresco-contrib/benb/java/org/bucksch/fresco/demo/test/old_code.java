			/* 0.0.1
			Graphic accountlist = cc.layout.vbox();
			C c = new C(cc);
			Account[] accounts = mailnews.accounts();
			for (int i = 0; i < accounts.length; i++)
			{
				OpenAccount commandImpl = new OpenAccount(accounts[i], c);
				objRef = cc.poa.servant_to_reference(commandImpl);
				Command commandRef = CommandHelper.narrow(objRef);
				Trigger open_button = cc.widget.button(c.label(accounts[i].name()),
																							 commandRef);
				accountlist.append_graphic(open_button);
			}
			cc.desktop.shell(cc.tool.group(c.background(accountlist)));
			*/
			/*
			Graphic transformed = cc.figure.transformer(
																			 	cc.layout.lmargin(container, 150.));
			transformer = transformed.transformation();
			Warsaw.TransformPackage.MatrixHolder matrix =
				                           new Warsaw.TransformPackage.MatrixHolder();
			transformer.store_matrix(matrix);
			matrix.value[1][0] = 0.01;
			//			matrix.value[1][1] = 0.01;
			//			matrix.value[1][2] = 0.01;
			transformer.load_matrix(matrix.value);
      //Vertex scale = new Vertex(1.,1.,1.);
			//			transformer.scale(scale);
						//transformer.translate(scale);
			//transformer.rotate(30., Axis.xaxis);
			representation.append_graphic(transformed);
			*/
