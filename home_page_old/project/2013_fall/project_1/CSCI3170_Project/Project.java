import java.sql.*;
import java.util.*;

//import StateMachine.class

public class Project{

	Connection conn;
	String username;
	String password;
	String reminder1;
	String reminder_Data_Mani;
	String reminder_Gen_Data;
	String reminder3;
	//SQL STATEMENT //
	String SQL_create_category, SQL_delete_category, SQL_show_category;
	String SQL_create_manufacturer, SQL_delete_manufacturer, SQL_show_manufacturer;
	String SQL_create_part, SQL_delete_part, SQL_show_part;
	String SQL_create_salesperson,SQL_delete_salesperson, SQL_show_salesperson;
	String SQL_create_transaction,SQL_delete_transaction, SQL_show_transaction;
	
	String SQL_query_1,SQL_query_2,SQL_query_3,SQL_query_4,SQL_query_5;
	//-------------//
	Scanner in;
	public static void main(String args[])
	{
		Project P = new Project();
		return ;
	}
	
	public Project()
	{
		username = new String("c056");
		password = new String("csci3170");
		reminder1 = new String("\nWhat operation would you like to perform?\n1. Data Manipulation\n2. General Data Operation\nEnter your choice:");
		//REMINDER 2
		reminder_Data_Mani = new String("What operation would you like to perform?\n");
		reminder_Data_Mani += "1. Create All Tables\n";
		reminder_Data_Mani += "2. Delete All Tables\n";
		reminder_Data_Mani += "3. Load Data into Tables\n";
		reminder_Data_Mani += "4. Show information in Tables\n";
		reminder_Data_Mani += "Enter your Choice:";
		//---------------------//
		//REMINDER 3 FOR DATA INPUT
		reminder3 = new String("\nWhich is the target table?\n");
		reminder3 += "1. Category\n";
		reminder3 += "2. Manufacturer\n";
		reminder3 += "3. Part\n";
		reminder3 += "4. Salesperson\n";
		reminder3 += "5. Transacation Record\n";
		reminder3 += "Type in the Target Table: ";
		in = new Scanner(System.in);
		initSQL();
		Connect();
		System.out.printf("Welcome to Sales System!\n");
		while(true)
		{
			StateMachine();
		}
	}

	public void Connect()
	{
		//Test the driver
		try{Class.forName("oracle.jdbc.driver.OracleDriver");}
		catch (Exception X)
		{
		System.out.println("Unable to load the driver class!\n");
		}
		//Create Connection
		try{
		conn = DriverManager.getConnection("jdbc:oracle:thin:@db12.cse.cuhk.edu.hk:1521:db12",username,password); 
		}
		catch (SQLException E)
		{
			E.printStackTrace();
		}
		System.out.printf("Connection Established\n");
	}
	
	public void StateMachine()
	{	
		//Print reminder and read in command
		System.out.printf("\n\n%s",reminder1);
		String command = in.nextLine();
		//System.out.printf("You've enter:%s\n",command);

		//Compare command and proceed
		if(command.equals("1") )
		{
			//System.out.printf("You choose 1\n");
			DataManipulation();
		}
		else if(command.equals("2"))
		{
			//System.out.printf("You choose 2\n");
			GeneralDataOperation();
		}
		else
		System.out.printf("Invalid Choice\n");
	}
	
	//FUNCTION FOR DATA MANIPULATION
	void DataManipulation()
	{
		System.out.printf("%s",reminder_Data_Mani);
		while(true)
		{
			String command = in.nextLine();
			if(command.equals("1"))
			{
				//CASE FOR CREATE ALL TABLES
				//System.out.printf("CASE FOR CREATE ALL TABLES\n");
				if(CreateAllTables() > 0)
				System.out.printf("Done.\n\n");
				else
				System.out.printf("Error Happened.\n\n");
				return ;
			}
			else if(command.equals("2"))
			{
				//CASE FOR DELETE ALL TABLES
				//System.out.printf("CASE FOR DELETE ALL TABLES\n");
				if(DeleteAllTables() > 0)
				System.out.printf("Done.\n\n");
				else
				System.out.printf("Error Happened.\n\n");
				return ;
			}
			else if(command.equals("3"))
			{
				//CASE FOR Load Data into Tables
				//System.out.printf("CASE FOR LOAD DATA INTO TABLES\n");
				LoadDataToTables();
				return ;
			}
			else if(command.equals("4"))
			{
				//CASE FOR SHOWING INFROMATION IN TABLES
				//System.out.printf("CASE FOR SHOW INFORMATION IN TABLES\n");
				ShowInfoInTables();
				return ;
			}
			else if(command.equals("exit"))
			return ;
			else ;
		}
	}
	
	int CreateAllTables()
	{
		int error = 0;
		System.out.printf("Processing...");
		Statement temp;
		try{temp = conn.createStatement();}
		catch(SQLException E)
		{System.out.println(E.getMessage()); return -1;}
	
		//CREATE CATEGORY TABLE;
		try{temp.executeUpdate(SQL_create_category);}
		catch(SQLException E)
		{System.out.printf("\ncategory: %s", E.getMessage()); error = 1;}
				
		//CREATE MANUFACTURER TABLE;
		try{temp.executeUpdate(SQL_create_manufacturer);}
		catch(SQLException E)
		{System.out.printf("\nmanufacturer: %s", E.getMessage()); error = 1;}
		
		//CREATE PART TABLE;
		try{temp.executeUpdate(SQL_create_part);}
		catch(SQLException E)
		{System.out.printf("\npart: %s", E.getMessage()); error = 1;}	
		
		//CREATE SALESPERSON TABLE;
		try{temp.executeUpdate(SQL_create_salesperson);}
		catch(SQLException E)
		{System.out.printf("\nsalesperson: %s", E.getMessage()); error = 1;}
		
		//CREATE TRANSACTION TABLE;
		try{temp.executeUpdate(SQL_create_transaction);}
		catch(SQLException E)
		{System.out.printf("\ntransaction: %s\n", E.getMessage()); error = 1;}
		
		if(error == 1)
		return -1;
		else
		return 1;
	}
	int DeleteAllTables()
	{
		int error = 0;
		System.out.printf("Processing...");
		Statement temp;
		try{temp = conn.createStatement(); }
		catch(SQLException E)
		{System.out.printf("CREATE STATEMENT: %s\n" + E.getMessage()); return -1;}
		
		try{temp.executeUpdate(SQL_delete_transaction);}
		catch(SQLException E)
		{System.out.printf("\ndelete transcation: %s", E.getMessage()); error = 1;}
		
		try{temp.executeUpdate(SQL_delete_salesperson);}
		catch(SQLException E)
		{System.out.printf("\ndelete salesperson: %s", E.getMessage()); error = 1;}
		
		try{temp.executeUpdate(SQL_delete_part);}
		catch(SQLException E)
		{System.out.printf("\ndelete part: %s", E.getMessage()); error = 1;}
		
		try{temp.executeUpdate(SQL_delete_manufacturer);}
		catch(SQLException E)
		{System.out.printf("\ndelete manufacturer: %s",E.getMessage()); error = 1;}
		
		try{temp.executeUpdate(SQL_delete_category);}
		catch(SQLException E)
		{System.out.printf("\ndelete category:  %s", E.getMessage()); error = 1;}

		if(error == 1)
		return -1;
		else
		return 1;
	}
	
	
	
	int LoadDataToTables()
	{
		System.out.printf("\nType in the Source Data File Path:");
		String path = in.nextLine();
		java.io.File file = new java.io.File(path);
                //CHECK THE FILE EXISTENCE
		if(file.exists())
                {//System.out.printf("The file exists\n");}
		;}
                else
                {System.out.printf("The file doesn't exist\n"); return -1;}
		//IF THE FILE EXIST, CONTINUES
		System.out.printf("%s", reminder3);
		String command = in.nextLine();
		//------------------------------//
		Scanner input;
		try{input = new Scanner(file);}
		catch(Exception E){System.out.printf("%s\n", E.getMessage()); return -1;}
		//------------------------------//
		if(command.equals("1"))
		{
			//CASE FOR READING A CATEGORY TABLE
			String record;
			while(input.hasNext())
			{
				record = input.nextLine();
				String cID_char;
				String cName_char;
				cID_char = record.substring(0,record.indexOf("	"));
				cName_char = record.substring(record.indexOf("	"));
				//TODO: SHALL I DISCARD THE CHANGES MADE IF ONE FORMAT ERROR IS DETECTED?
				if(cID_char.length() != 1)
				{System.out.printf("Error format for category file!\n\n"); return -1;}
				if(cName_char.length() > 50)
				{System.out.printf("Error format for category file!\n\n"); return -1;}
				//System.out.printf("cID:%s	cName:%s\n",cID_char,cName_char);
				
			}
			input.close();
				
			//READY TO INSERT VALUES INTO TABLES
			Statement temp;
			try{ temp  = conn.createStatement();}
			catch(Exception E){System.out.printf("Cannot create statement: %s\n", E.getMessage()); return -1;}
			
			System.out.printf("\nProccessing.......");
			try{input = new Scanner(file);}
			catch(Exception E){System.out.printf("%s\n", E.getMessage()); return -1;}
			while(input.hasNext())
                        {
				//System.out.printf("Read again\n");
                                record = input.nextLine();
				//System.out.printf("RECORD:%s\n",record);
                                String cID_char;
                                String cName_char;
                                cID_char = record.substring(0,record.indexOf("	"));
                                cName_char = record.substring(record.indexOf("	"));

				try{temp.executeUpdate("INSERT INTO category VALUES(" + cID_char + ",'" + cName_char + "')");}
				catch(SQLException E)
				{System.out.printf("Error Inserting ID:%s NAME:%s :%s\n",cID_char,cName_char,E.getMessage());}		
                        }
			System.out.printf("Done\n\n");
		}
		else if(command.equals("2"))
		{
			//CASE FOR READING A MANUFACTURER TABLE
			String record;
			Statement temp;
			try{ temp = conn.createStatement();}
			catch(Exception E){System.out.printf("Cannot create statement: %s\n", E.getMessage()); return -1;}
			
			for(int count = 0;count < 2;count++)
			{
				if(count == 1)
				try{input = new Scanner(file);}
				catch(Exception E)
				{System.out.printf("Error Opening file %s\n",E.getMessage());}
				
				System.out.printf("Processing.......");
				while(input.hasNext())
				{
					record = input.nextLine();
					String mID, name, address, phoneNumber;
				
					mID = record.substring(0,record.indexOf("	"));
					record = record.substring(record.indexOf("	") + 1);
					//System.out.printf("%s\n", record);
					if(record.length() == 0)
					{System.out.printf("Error format for manufacturer file!a\n"); return -1;}
				
					name = record.substring(0,record.indexOf("	"));
					record = record.substring(record.indexOf("	") + 1);
					//System.out.printf("%s\n", record);
					if(record.length() == 0)
					{System.out.printf("Error format for manufacturer file!b\n"); return -1;}
				
					address = record.substring(0,record.indexOf("	"));
					record = record.substring(record.indexOf("	") + 1);
					//System.out.printf("%s\n",record);
                                	if(record.length() == 0)
                                	{System.out.printf("Error format for manufacturer file!c\n"); return -1;}
				
					phoneNumber = record;
				
					//CHECK FOR FIELDS' LENGTH 
					if(!(mID.length() == 1 || mID.length() == 2)  || name.length() > 20 || address.length() > 50 || phoneNumber.length() > 8 || phoneNumber.length() == 0)
					{
						System.out.printf("Error format for manufacturer file!\n");
						System.out.printf("%d %d %d %d\n", mID.length(), name.length(), address.length(), phoneNumber.length());
						System.out.printf("%s %s %s %s\n",mID,name,address,phoneNumber);
						 return -1;}
					
					//FOR THE SECOND ROUND EXECUTE SQL STATEMENT
					if(count == 1)
					{	try
						{
							temp.executeUpdate("INSERT INTO manufacturer VALUES(" + 
									mID + ",'" + name + "'," + "'" + address + "'," + phoneNumber + ")");
						}
						catch (SQLException E)
						{System.out.printf("Error Inserting %s %s %s %s: %s\n",mID,name,address,phoneNumber,E.getMessage());}
					}				
				}
				System.out.printf("Done\n\n");
			input.close();
			}
		}
		else if(command.equals("3"))
		{
			//CASE FOR READING A PART TABLE
			String record;
			Statement temp;
		  	System.out.printf("Processing.....");	
			String pID, name, price, manufacturer_id, category_id, available_quantity;
			try{temp = conn.createStatement();}
			catch(Exception E){System.out.printf("Cannot create statement: %s\n", E.getMessage()); return -1;}
			while(input.hasNext())
			{
				record = input.nextLine();
				pID = record.substring(0,record.indexOf("	"));
				record = record.substring(record.indexOf("	") + 1);
				//System.out.printf("record: %s\n",record);	
				
				name = record.substring(0,record.indexOf("	"));
				record = record.substring(record.indexOf("	") + 1);
				//System.out.printf("record: %s\n",record);
				
				price = record.substring(0,record.indexOf("	"));
				record = record.substring(record.indexOf("	") + 1);
				//System.out.printf("record: %s\n",record);
				
				manufacturer_id = record.substring(0,record.indexOf("	"));
                                record = record.substring(record.indexOf("	") + 1);
				//System.out.printf("record: %s\n",record);
				
				category_id = record.substring(0,record.indexOf("	"));
                                record = record.substring(record.indexOf("	") + 1);
				
				available_quantity = record;
				
				String SQL_insert_statement = new String("INSERT INTO part VALUES" +
                                                        "(" + pID + ",'" + name + "'," + price + "," + manufacturer_id + "," +
                                                         category_id + "," + available_quantity + ")");

				try{temp.executeUpdate(SQL_insert_statement);}
				catch(SQLException E)
				{
				System.out.printf("\nError in %s: %s\n",SQL_insert_statement,E.getMessage());
				}
			}
			System.out.printf("Done\n\n");
		}
		else if(command.equals("4"))
		{
			//CASE FOR READING A SALESPERSON TABLE
			String record;
			Statement temp;
			String sID, name, address, phoneNumber;
			try{temp = conn.createStatement();}
			catch (Exception E){System.out.printf("cannot create statement: %s\n",E.getMessage());return -1;}
			System.out.printf("Procsssing........");
			while(input.hasNext())
			{
				record = input.nextLine();
				sID = record.substring(0,record.indexOf("	"));
				record = record.substring(record.indexOf("	") + 1);
			
				name = record.substring(0,record.indexOf("	"));
				record = record.substring(record.indexOf("	") + 1);
				
				address = record.substring(0,record.indexOf("	"));
				record = record.substring(record.indexOf("	") + 1);
				
				phoneNumber = record;
				
				String SQL_insert_statement = new String("INSERT INTO salesperson VALUES" + 
									"(" + sID + 
									",'" + name + "'," + 
									"'" + address + "'," + 
									phoneNumber + ")" );
				try{temp.executeUpdate(SQL_insert_statement);}
				catch(SQLException E)
				{
				System.out.printf("\nError in %s: %s\n",SQL_insert_statement, E.getMessage());
				}
			}
			System.out.printf("Done\n\n");
		}
		else if(command.equals("5"))
		{
			//CASE FOR READING A TRANSACTION TABLE
			String record;
			Statement temp;
			String tID, pID, sID, tdate;
			try{temp = conn.createStatement();}
			catch(Exception E){System.out.printf("Cannot Create Statement %s\n", E.getMessage());return -1;}
			System.out.printf("Processing........");
			while(input.hasNext())
			{
				record = input.nextLine();
				tID = record.substring(0,record.indexOf("	"));
				record = record.substring(record.indexOf("	") + 1);
				
				pID = record.substring(0,record.indexOf("	"));
				record = record.substring(record.indexOf("	") + 1);
				
				sID = record.substring(0,record.indexOf("	"));
				record = record.substring(record.indexOf("	") + 1);

				tdate = record;
					
				String SQL_insert_statement = new String("INSERT INTO transaction VALUES" + 
									"(" + tID + "," + 
									pID + "," + 
									sID + "," + 
									"to_date(" + "'" + tdate + "'," + "'DD/MM/YYYY" + "'))");
				
				try{temp.executeUpdate(SQL_insert_statement);}
				catch(SQLException E){System.out.printf("\nError in %s :%s\n",SQL_insert_statement,E.getMessage());}
			}
			System.out.printf("Done\n\n");
		}
		System.out.printf("\n");
		return 1;
	}
	


	
	int ShowInfoInTables()
	{
		System.out.printf("Which table do you want to show?\n1. Category\n2. Manufacturer\n3. Part\n4. Salesperson\n 5. Transaction Record\nType in the Target Table:");
		String command = in.nextLine();
		if(command.equals("1"))
		return ShowCategory();
		else if(command.equals("2"))
		return ShowManufacturer();
		else if(command.equals("3"))
		return ShowPart();
		else if(command.equals("4"))
		return ShowSalesperson();
		else if(command.equals("5"))
		return ShowTransaction();
		else
		{
		 	//TODO ERROR MESSAGE
			return 1;
		}	
	}
	

	//FUNCTION TO DISPLAY THE CATEGORY
	int ShowCategory()
	{
		Statement temp;
		try{temp = conn.createStatement();}
		catch(Exception E)
		{System.out.printf("Cannot create statement:%s\n",E.getMessage());return -1;}
		
		ResultSet result;
		try {result  = temp.executeQuery(SQL_show_category);}
		catch(SQLException E)
		{System.out.printf("Error in %s :%s\n",SQL_show_category,E.getMessage()); return -1;}
		String cID = "ID", name = "name";
		System.out.printf("|%-3s|%-20s|\n",cID,name);
		while(true)
		{
			try{result.next();cID = result.getString(1); name = result.getString(2);}
			catch(SQLException E){break;}
			System.out.printf("|%-3s|%-20s|\n",cID,name);
		}	
		
		return 1;	
	}
	
		
	//FUNCTION TO DISPLAY THE MANUFACTURER	
	int ShowManufacturer()
	{
		Statement temp;
                try{temp = conn.createStatement();}
                catch(Exception E)
                {System.out.printf("Cannot create statement:%s\n",E.getMessage());return -1;}

                ResultSet result;
                try {result  = temp.executeQuery(SQL_show_manufacturer);}
                catch(SQLException E)
                {System.out.printf("Error in %s :%s\n",SQL_show_manufacturer,E.getMessage()); return -1;}
		String mID  = "mID", name = "name", address = "addresss", phoneNumber = "phoneNumber";
                System.out.printf("|%-3s|%-20s|%-50s|%-8s|\n",mID,name,address,phoneNumber);
		
		while(true)
		{
		
			try{
				result.next();
				mID = result.getString(1); 
				name = result.getString(2);
				address = result.getString(3);
				phoneNumber = result.getString(4);
			}
                        catch(SQLException E){break;}
                        System.out.printf("|%-3s|%-20s|%-50s|%-8s|\n",mID,name,address,phoneNumber);

		}	
		return 1;
	}

	int ShowPart()
	{
		Statement temp;
                try{temp = conn.createStatement();}
                catch(Exception E)
                {System.out.printf("Cannot create statement:%s\n",E.getMessage());return -1;}

                ResultSet result;
                try {result  = temp.executeQuery(SQL_show_part);}
                catch(SQLException E)
                {System.out.printf("Error in %s :%s\n",SQL_show_part,E.getMessage()); return -1;}
                String pID  = "pID", name = "name", price = "price", mID = "mID", cID = "cID", avail_quan = "Available Quantity";
                System.out.printf("|%-3s|%-20s|%-5s|%-3s|%-3s|%-2s|\n",pID,name,price,mID,cID,avail_quan);

                while(true)
                {

                        try{
                                result.next();
                                pID = result.getString(1);
                                name = result.getString(2);
                               	price = result.getString(3);
                                mID = result.getString(4);
				cID = result.getString(5);
				avail_quan = result.getString(6);
                        }
                        catch(SQLException E){break;}
			System.out.printf("|%-3s|%-20s|%-5s|%-3s|%-3s|%-2s|\n",pID,name,price,mID,cID,avail_quan);

                }

		return 1;
	}
	
	int ShowSalesperson()
	{
		Statement temp;
                try{temp = conn.createStatement();}
                catch(Exception E)
                {System.out.printf("Cannot create statement:%s\n",E.getMessage());return -1;}

                ResultSet result;
                try {result  = temp.executeQuery(SQL_show_salesperson);}
                catch(SQLException E)
                {System.out.printf("Error in %s :%s\n",SQL_show_salesperson,E.getMessage()); return -1;}
		
		String sID = "sID", name = "name", address = "address", phoneNumber = "phoneNumber";
		System.out.printf("|%-3s|%-20s|%-50s|%-8s|\n",sID,name,address,phoneNumber);
		
		while(true)
		{
			try{
				result.next();
				sID = result.getString(1);
				name = result.getString(2);
				address = result.getString(3);
				phoneNumber = result.getString(4);
			}
 			//TODO MAYBE TRY TO DETECT ERROR EXCEPT FOR Exhausted Result set
			catch(SQLException E){break;}
			System.out.printf("|%-3s|%-20s|%-50s|%-8s|\n",sID,name,address,phoneNumber);
			
		}

		return 1;
	}

	int ShowTransaction()
	{
		Statement temp;
                try{temp = conn.createStatement();}
                catch(Exception E)
                {System.out.printf("Cannot create statement:%s\n",E.getMessage());return -1;}

                ResultSet result;
		try {result = temp.executeQuery("SELECT to_char(sysdate, '" + "dd/mm/yyyy') datetime from dual");}
		catch(SQLException E)
		{System.out.printf("Error in formating date:%s\n",E.getMessage());return -1;}
                try {result  = temp.executeQuery(SQL_show_transaction);}
                catch(SQLException E)
                {System.out.printf("Error in %s :%s\n",SQL_show_transaction,E.getMessage()); return -1;}
		
		String tID = "tID", pID = "pID", sID = "sID", tDate = "tDate";
		System.out.printf("|%-4s|%-3s|%-3s|%-12s|\n",tID,pID,sID,tDate);
		
		while(true)
		{
			try{
				result.next();
				tID = result.getString(1);
				pID = result.getString(2);
				sID = result.getString(3);
				tDate = result.getString(4);
			}
			catch(SQLException E){break;}
			System.out.printf("|%-4s|%-3s|%-3s|%-12s|\n",tID,pID,sID,tDate);
		}
		return 1;
	}
	
		
		
	int GeneralDataOperation()
	{ 
		System.out.printf("\nWhat operation would you like to perform?\n1. Search and list Parts \n2. Enter a new transaction\n3. Show the transaction history\n4. Rank category sales in ascending order \n5. Rank salespersons sales in ascending order\nEnter your choice:");
		String command = in.nextLine();
		if(command.equals("1"))
		SearchAndListParts();
		else if(command.equals("2"))
		EnterANewTransaction();
		else if(command.equals("3"))
		ShowTheTransactionHistory();
		else if(command.equals("4"))
		RankCategorySalesInASCOrder();
		else if(command.equals("5"))
		RankSalespersonInASCOrder();
		else 
		System.out.printf("\n\nERROR: INVALID INPUT\n\n\n");
		return 1;
	}
 
        //SERACH AND LIST ALL PARTS	
	int SearchAndListParts()
	{
		System.out.printf("Choose the Search criterion: \n1. Category Name \n2. Manufacturer Name \n3. Part Name");
		System.out.printf("\nEnter your Choice:");
		String command = in.nextLine();
		ResultSet result;		
		if(command.equals("1"))
		{	
			//CASE FOR SERACHING FOR CATEGORY AS KEY WORD
			System.out.printf("Type in the Search Keyword:");
			command = in.nextLine();
			
			Statement temp;
			try{temp = conn.createStatement();}
			catch(Exception E){System.out.printf("Cannot create Statement:%s\n",E.getMessage());return -1;}
			
			//ResultSet result;
			String Exact_Query = SQL_query_1 + "AND C.name LIKE '%" + command + "%'";
			try{result = temp.executeQuery(Exact_Query);}
			catch(SQLException E)
			{System.out.printf("\nError in %s: %s\n\n",Exact_Query,E.getMessage()); return -1;}
		}
		else if(command.equals("2"))
		{
			 //CASE FOR SERACHING FOR MANUFACTURER NAME AS KEY WORD
                        System.out.printf("Type in the Search Keyword:");
                        command = in.nextLine();

                        Statement temp;
                        try{temp = conn.createStatement();}
                        catch(Exception E){System.out.printf("Cannot create Statement:%s\n",E.getMessage());return -1;}

                        //ResultSet result;
                        String Exact_Query = SQL_query_1 + "AND M.name LIKE '%" + command + "%'";
                        try{result = temp.executeQuery(Exact_Query);}
                        catch(SQLException E)
                        {System.out.printf("\nError in %s: %s\n\n",Exact_Query,E.getMessage()); return -1;}

		}
		else if(command.equals("3"))
		{
			 //CASE FOR SERACHING FOR PART NAME AS KEY WORD
                        System.out.printf("Type in the Search Keyword:");
                        command = in.nextLine();

                        Statement temp;
                        try{temp = conn.createStatement();}
                        catch(Exception E){System.out.printf("Cannot create Statement:%s\n",E.getMessage());return -1;}

                        //ResultSet result;
                        String Exact_Query = SQL_query_1 + "AND P.name LIKE '%" + command + "%'";
                        try{result = temp.executeQuery(Exact_Query);}
                        catch(SQLException E)
                        {System.out.printf("\nError in %s: %s\n\n",Exact_Query,E.getMessage()); return -1;}

		}
		else 
		{
			System.out.printf("\n\nERROR: INVALID INPUT\n\n\n");
			return -1;
		}
		
		String pid = "Part ID",pname = "Part Name",cname = "Category",mname = "Manufacturer",avail_quan = "Available Quantity";		
		//DISPLAY QUERY RESULT
		System.out.printf("|%-3s|%-20s|%-20s|%-20s|%-2s|\n",pid,pname,cname,mname,avail_quan);
		while(true)
		{
			try
			{
				result.next();
				pid = result.getString(1);
				pname = result.getString(2);
				cname = result.getString(3);
				mname = result.getString(4);
				avail_quan = result.getString(5);
			}
			catch (SQLException E){break;}
			if(avail_quan.equals("00") || avail_quan.equals("0"))
			continue;
			
			System.out.printf("|%-7s|%-20s|%-20s|%-20s|%-2s|\n",pid,pname,cname,mname,avail_quan);

		}
		System.out.printf("End Of Query Result\n\n\n");
		return 1;
	}
	
	int EnterANewTransaction()
	{
		//ASK THE USER TO ENTER NECESSARY DATA
		System.out.printf("Enter Your Sales ID:");
		String sID = in.nextLine();
		System.out.printf("Enter Your Item ID:");
		String pID = in.nextLine();	
		//
		
		Statement temp;
		try{temp = conn.createStatement();}
		catch(Exception E)
		{System.out.printf("\nCannot Create Statement:%s\n", E.getMessage()); return -1;}
		
		String Exact_Query = SQL_query_2 + pID + "," + sID + "," + "SYSDATE)";
		
		ResultSet result;
		try{result = temp.executeQuery("SELECT available_quantity from part WHERE pid = " + pID);}
		catch(SQLException E)
		{
			System.out.printf("\nError in SELECT available_quantity from part WHERE pid = pID: %s\n",E.getMessage());
			return -1;
		}
		String Avail_quan;
		try{
			result.next();
			Avail_quan = result.getString(1);
		}
		catch(SQLException E){System.out.printf("No available quantity record found:%s\n\n",E.getMessage()); return -1;}
		System.out.printf("Avail_Quan = %s\n", Avail_quan);	
		//FINSIHED CHECK WHETHER THE AVAIL QUANTITY IS 0
		if(!Avail_quan.equals("0"))
		{
			//System.out.printf("TRYING TO INSERT THE TRANSACTION\n");
			try{temp.executeUpdate(Exact_Query);}
			catch(SQLException E)
			{
				System.out.printf("\nError in %s: %s\n",Exact_Query,E.getMessage());
				return -1;
			}
			String reduce_part = new String("UPDATE part SET available_quantity = (available_quantity - 1) WHERE pid = " + pID);
			//System.out.printf("TRYING TO UPDATE THE PART AVAILABLE QUANTITY pid:%s\n\n",pID);
			//System.out.printf("%s\n",reduce_part);
			try{temp.executeUpdate(reduce_part);}
			catch(SQLException E)
			{
				System.out.printf("\nError in %s: %s\n",reduce_part,E.getMessage());
				return -1;
			}
			//System.out.printf("FLAG 2\n");
		}
		else
		{
			System.out.printf("Error : The available quantity of the part is 0\n\n");
			return -1;
		}

		return 1;
	}
	
	int ShowTheTransactionHistory()
	{
		System.out.printf("Choose the Search criterion:\n1. Salesperson ID\n2. Part ID\nEnter your Choice:");
		String command = in.nextLine();
		Statement temp;
		try{temp = conn.createStatement();}
		catch(Exception E){System.out.printf("Cannot create statement\n", E.getMessage()); return -1;}
		
			//
			System.out.printf("Type in the Serach Key:");
			String key = in.nextLine();
			String exact_Query;
			if(command.equals("1"))
			exact_Query = SQL_query_3 + "AND S.sID = " + key;
			else if (command.equals("2"))
			exact_Query = SQL_query_3 + "AND P.pid = " + key;
			else
			{
				System.out.printf("Error: Invalid output\n");
				return -1;
			}
			ResultSet result;
			try{result = temp.executeQuery(exact_Query);}
			catch(SQLException E){System.out.printf("Error in %s: %s\n",exact_Query,E.getMessage()); return -1;}
			String tid = "Transaction ID", pid = "Part ID", pname = "Part Name", sid = "Salesperson ID", sname = "Salesperson Name", tDate = "Transaction Date";			
			System.out.printf("|%-20s|%-10s|%-20s|%-20s|%-20s|%-20s|\n",tid,pid,pname,sid,sname,tDate);
			while(true)
			{
				try{
					result.next();
                                        tid = result.getString(1);
                                        pid = result.getString(2);
                                        pname = result.getString(3);
                                        sid = result.getString(4);
                                        sname = result.getString(5);
                                        tDate = result.getString(6);

				}
				catch(SQLException E){System.out.printf("End of the Query\n\n");break;}
				
				System.out.printf("|%-20s|%-10s|%-20s|%-20s|%-20s|%-20s|\n",tid,pid,pname,sid,sname,tDate);
			}		
		return 1;
	}
	
	int RankCategorySalesInASCOrder()
	{
		Statement temp;
		try{temp = conn.createStatement();}
		catch(SQLException E){System.out.printf("Cannot Creat Statement\n",E.getMessage()); return -1;}
			
		ResultSet result;
		try{result = temp.executeQuery(SQL_query_4);}
		catch(SQLException E){System.out.printf("Error in %s :%s\n",SQL_query_4,E.getMessage());return -1;}
		String cid = "Category ID", cname = "Category Name", value = "Total Sales Value";
		System.out.printf("|%-20s|%-20s|%-20s|\n",cid,cname,value); 
		while(true)
		{
			try{
				result.next();
				cid = result.getString(1);
				cname = result.getString(2);
				value = result.getString(3);	
			}
			catch(SQLException E)
			{
				break;
			}
			System.out.printf("|%-20s|%-20s|%-20s|\n",cid,cname,value);

		}
		System.out.printf("End of Query\n\n");
		return 1;
	}

	int RankSalespersonInASCOrder()
	{
		Statement temp;
                try{temp = conn.createStatement();}
                catch(SQLException E){System.out.printf("Cannot Creat Statement\n",E.getMessage()); return -1;}

                ResultSet result;
                try{result = temp.executeQuery(SQL_query_5);}
                catch(SQLException E){System.out.printf("Error in %s :%s\n",SQL_query_5,E.getMessage());return -1;}
                String sid = "Salesperson ID", sname = "Salesperson Name", value = "Total Sales Value";
                System.out.printf("|%-20s|%-20s|%-20s|\n",sid,sname,value);
                while(true)
                {
                        try{
                                result.next();
                                sid = result.getString(1);
                                sname = result.getString(2);
                                value = result.getString(3);
                        }
                        catch(SQLException E)
                        {
                                break;
                        }
                        System.out.printf("|%-20s|%-20s|%-20s|\n",sid,sname,value);

                }
                System.out.printf("End of Query\n\n");
                return 1;
	}
	void initSQL()
	{
		//CREATE TABLE STATEMENT
		SQL_create_category = new String("CREATE TABLE category" +
						"(cID Integer PRIMARY KEY, name varchar2(20), UNIQUE(name))");
		
		SQL_create_manufacturer = new String("CREATE TABLE manufacturer" + 
						"(mID Integer PRIMARY KEY, name varchar2(20),address varchar2(50),phoneNumber Integer)");

		SQL_create_part = new String("CREATE TABLE part" +
					"(pID Integer PRIMARY KEY, name varchar2(20),price Integer," + 
					"manufacturer_id Integer REFERENCES manufacturer(mID)," + 
					"category_id Integer REFERENCES category(cID)," + 
					"available_quantity Integer)");
		
		SQL_create_salesperson = new String("CREATE TABLE salesperson" + 
						"(sID Integer PRIMARY KEY, name varchar2(20), address varchar(50), phoneNumber Integer)");
		//TODO:NOT YET IMPLEMENT THE ONE TO ONE RELATIONSHIP FOR TRANSACTION AND PART	
		SQL_create_transaction = new String("CREATE TABLE transaction" + 
						"(tID Integer PRIMARY KEY," + 
						"pID Integer REFERENCES part(pID)," + 
						"sID Integer REFERENCES salesperson(sID)," +  
						"tDate date)");

		//DROP TABLE STATEMENT
		SQL_delete_category = new String("DROP TABLE category");
		SQL_delete_manufacturer = new String("DROP TABLE manufacturer");
		SQL_delete_salesperson = new String("DROP TABLE salesperson");
		SQL_delete_part = new String("DROP TABLE part");
		SQL_delete_transaction = new String("DROP TABLE transaction");
		
		//SHOE TABLE STATEMENT	
		SQL_show_category = new String("SELECT * FROM category");
		SQL_show_manufacturer = new String("SELECT * FROM manufacturer");
		SQL_show_salesperson = new String("SELECT * FROM salesperson");
		SQL_show_part = new String("SELECT * FROM part");
		SQL_show_transaction = new String("SELECT tid,pid,sid, to_char(tDate,'DD/MM/YYYY') FROM transaction");	
		
		//SQL FOR GENERAL OPERATION
		SQL_query_1 = new String("SELECT P.pid, P.name,C.name, M.name, P.available_quantity " + 
					"FROM part P, manufacturer M, category C " +
					"WHERE P.manufacturer_id = M.mid AND P.category_id = C.cid ");
		SQL_query_2 = new String("INSERT INTO transaction VALUES" + 
					"((SELECT (MAX(tID) + 1) FROM transaction),");
		
		SQL_query_3 = new String("SELECT T.tid, P.pid, P.name, S.sid, S.name, to_char(T.tDate,'DD/MM/YYYY" + "')" + 
					" FROM transaction T, part P, Salesperson S" + 
					" WHERE T.pid = P.pid AND T.sid = S.sid ");
		//TODO CHECK THE CORRECTNESS OF THE QUERY	
		SQL_query_4 = new String("SELECT C.cid, C.name, SUM(TEMP.TOTAL_VALUE_OF_PART) AS TOTAL_SALE " + 
					"FROM category C, " + 
					"( " + 
					"SELECT T.tid, T.pid, P.category_id, SUM(P.price) AS TOTAL_VALUE_OF_PART " +
					"FROM transaction T, part P " +
					"WHERE T.pid = P.pid " +
					"GROUP BY (T.tid, T.pid, P.category_id) " + 
					"ORDER BY T.pid " +
					") TEMP " + 
					"WHERE C.cid = TEMP.category_id " + 
					"GROUP BY (C.cid, C.name) " +
					"ORDER BY TOTAL_SALE ASC");

		//TODO CORRECTNESS OF THE QUERY NOT CHECK
		SQL_query_5 = new String("SELECT S.sid, S.name, SUM(TEMP.TOTAL_VALUE_OF_PART) AS Sale_Value " + 
					"FROM salesperson S, " + 
					"(" + 
					"SELECT T.sid, T.pid, SUM(P.price) AS TOTAL_VALUE_OF_PART " +
					"FROM transaction T, Part P " + 
					"WHERE P.pid = T.pid " +
					"GROUP BY (T.sid, T.pid) " + 
					") TEMP " +
					"WHERE S.sid = TEMP.sid " +
					"GROUP BY (S.sid, S.name) " +
					"ORDER BY Sale_Value");
	}

}
