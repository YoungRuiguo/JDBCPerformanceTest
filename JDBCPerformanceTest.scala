package example

import java.sql.{ResultSet, PreparedStatement, DriverManager}
import StreamBridge.getStream

import akka.stream._
import akka.stream.scaladsl._

import akka.{ NotUsed, Done }
import akka.actor.ActorSystem
import akka.util.ByteString
import scala.concurrent._
import scala.concurrent.duration._
import java.nio.file.Paths

import java.util.concurrent.Executors

import akka.{Done, NotUsed}
import akka.stream.ActorMaterializer
import scala.concurrent.Future

import java.sql.{PreparedStatement, Statement, Connection, ResultSet}

// write out test result
import java.io.{File, PrintWriter, FileOutputStream}
import java.util.Random

import scala.collection.mutable.{Map, SynchronizedMap, HashMap}

object JDBCPerformanceTest extends App {

  // Implicit variables for akka stream handling.
  implicit val system       = ActorSystem("Sys")
  implicit val materializer = ActorMaterializer()

  var timeFlow = Map[Int, List[Int]]()

  // try{
  var Driver = "com.mysql.jdbc.Driver"
  var Url = "jdbc:mysql://localhost:3306/test?autoReconnect=true&useSSL=false";
  var UserName = "admin"
  var Password = "123"
  val QUERY = """select name,email from user_tab order by id limit 10000;"""

  Class.forName(Driver)
  val conn = DriverManager.getConnection(Url, UserName, Password)

  // while(rs.next())
  //   println(s"""state = ${rs.getString("a.state")},name = ${rs.getString("b.name")},city = ${rs.getString("a.city")},population = ${rs.getString("a.population")}""")

  val resultSet = StreamBridge

  def mapRow(rs: ResultSet) : String = {
    return s"name = ${rs.getString("name")}, email = ${rs.getString("email")}"
  }

  def writeLine(line: String) : Unit = {
    val filePath = "/Users/ruiguo.yang/Documents/Lab/mysql/batch10000-5000.txt"
    val write = new PrintWriter(new FileOutputStream(new File(filePath),true))
    write.println(line)
    write.close()
  }

  var i = 0
  for(i <- 1 to 10) {
    val poolSize = 1
    val pool = java.util.concurrent.Executors.newFixedThreadPool(poolSize)
    val t0 = System.nanoTime()
    var t1 = System.nanoTime()
    timeFlow += (i -> List[Int]())
    timeFlow(i) :+ t0
    println(s"Start: loop = ${i}, timestamp = ${t0}")
    writeLine(s"Start: loop = ${i}, timestamp = ${t0}")
    1 to poolSize foreach {
      x => {
        pool.execute{
          new Runnable {
            def run{
              def printCurr(str: String) : Unit = {
                t1 = System.nanoTime()
                timeFlow(i) :+ t1
                println(s"Process: loop = ${i}, timestamp = ${t1 - t0}")
                writeLine(s"Process: loop = ${i}, timestamp = ${t1 - t0}")
              }
              val row = resultSet.getStream[String](QUERY, conn, 5000)(mapRow)(ECBlocking(ExecutionContext.global))
              val consoleSink: Sink[String, Future[Done]] = Sink.foreach[String](printCurr)
              row.runWith(consoleSink)
            }
          }
        }
      }
    }
  }
}
