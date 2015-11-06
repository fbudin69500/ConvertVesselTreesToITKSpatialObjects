//Program that creates a spatial object and transforms it into a mask image
//Wrote to test how to use spatial objects and to see if it is possible to convert vessels tre files into mask images.
#include "itkImage.h"
//#include "itkImageFileWriter.h"
#include "itkTubeSpatialObject.h"
#include "itkTubeSpatialObjectPoint.h"
#include "VesselsClass.h"
#include "itkSpatialObjectWriter.h"
#include "itkSceneSpatialObject.h"
#include <itkSpatialObjectProperty.h>

int main(int argc, char* argv[])
{
    //Load command line Arguments
    if( argc != 3 )
    {
        std::cerr << "Wrong number of arguments: " << argv[0] << " VesselsTreeFileName SOFile" << std::endl ;
        return 1 ;
    }

    //Load Vessels
    VesselsClass vessels;
    vessels.SetFileName( argv[1] );
    vessels.SetSampling( 1 ) ;
    vessels.Update();
    if( !vessels.IsValid() )
    {
        std::cerr << "Unable to read vessels file" << std::endl ;
        return 2 ;
    }
    eb::VESSCOL vesselCollection = vessels.GetOutput() ;
    //Get size and spacing of the vessels
    typedef itk::Image< unsigned char , 3 > MaskImageType ;
    MaskImageType::SizeType size ;
    MaskImageType::SpacingType spacing ;
    for( int i = 0 ; i < 3 ; i++ )
    {
      spacing[i] = vesselCollection.GetSteps(i);
      size[i] = vesselCollection.GetDims(i);
    }
    //Create Spatial Object list
    typedef itk::SceneSpatialObject<3> SceneSpatialObjectType;
    SceneSpatialObjectType::Pointer scene = SceneSpatialObjectType::New();
    typedef itk::TubeSpatialObject< 3 > TubeType ;
    typedef TubeType::PropertyType PropertyType ;
    int nbVessels = vesselCollection.GetVessnum() ;
    //std::vector< TubeType::Pointer > tree ;
    //tree.resize( nbVessels ) ;
    float* colorPointer ;
    typedef itk::TubeSpatialObjectPoint< 3 > TubePointType ;
//    std::vector< PropertyType::Pointer > vesselPropertyVec( nbVessels , NULL ) ;
    for( int i = 0 ; i < nbVessels ; i++ )
    {
       PropertyType::Pointer vesselProperty = PropertyType::New() ;
        vesselCollection.ResetOffset( i ) ;
        TubeType::Pointer tube = TubeType::New() ;
        TubeType::PointListType list ;
        colorPointer = vesselCollection.GetColor( i ) ;
        vesselProperty->SetColor( colorPointer[ 0 ] , colorPointer[ 1 ] , colorPointer[ 2 ] ) ;
        vesselProperty->SetAlpha( vesselCollection.GetOpacity( i ) ) ;
//        vesselPropertyVec[ i ] = vesselProperty ;
//        tube->SetProperty( vesselPropertyVec[ i ] ) ;
        tube->SetProperty( vesselProperty ) ;
        for( int j = 0 ; j < vesselCollection.NPoints(i) ; j += 1 ) //resample vessels )
        {
            TubePointType p ;
            float* fptr ;
            fptr = vesselCollection.Point(i, j);
            p.SetPosition( (*fptr) / spacing[0] , (*( fptr + 1 ) ) / spacing[1] , (*( fptr + 2 ) ) / spacing[2] ) ;
            p.SetRadius( *(fptr+3) / spacing[0] ) ;
            list.push_back( p ) ;
        }
        tube->SetId( i ) ;
        tube->SetPoints( list ) ;
        tube->ComputeTangentAndNormals() ;
        tube->Update() ;
        //tree[i] = tube ;
        scene->AddSpatialObject( tube ) ;
    }
    for( int i = 0 ; i < 3 ; i++ )
    {
      spacing[i] *= 10 ;
    }
    //Save spatial object
    typedef itk::SpatialObjectWriter<3> SOWriterType;
    SOWriterType::Pointer soWriter = SOWriterType::New();
    soWriter->SetInput( scene );
    soWriter->SetFileName( argv[2] );
    soWriter->Update();

    //Create an empty image for the mask
 /*   typedef itk::Image< unsigned char , 3 > MaskImageType ;
    MaskImageType::Pointer maskImage = MaskImageType::New() ;
    maskImage->SetRegions( size ) ;
    maskImage->SetSpacing( spacing ) ;
    maskImage->Allocate() ;
    maskImage->FillBuffer( 0 );*/
    //Check if the points belong to the spatial object. If so set the mask to 1, otherwise leave the mask to 0;
/*    itk::Point<double,3> insidePoint;
    //MaskImageType::IndexType pixelIndex ;
    //Goes over all tubes and fill the inside
    typedef TubeType::BoundingBoxType BoundingBoxType;
    BoundingBoxType::PointType lowerCorner ;
    BoundingBoxType::PointType higherCorner ;
    BoundingBoxType::Pointer boundingBox = BoundingBoxType::New() ;
    nbVessels = scene->GetNumberOfObjects() ;//just to verify
    for( int i = 0 ; i < nbVessels ; i++ )
    {
        itk::SpatialObject<3>::Pointer so = scene->GetObjectById( i ) ;
        //boundingBox = tree[i]->GetBoundingBox() ;
        //boundingBox = scene->GetObjectById( i )->GetBoundingBox() ;
        boundingBox = so->GetBoundingBox() ;
        lowerCorner = boundingBox->GetMinimum() ;
        higherCorner = boundingBox->GetMaximum() ;
        for( int k = 0 ; k < 3 ; k++ )
        {
            if( lowerCorner[k] < 0 )
            {
                lowerCorner[k] = 0 ;
            }
            if( higherCorner[k] >= size[k] )
            {
                higherCorner[k] = size[k] - 1 ;
            }
}*/
        //MaskImageType::IndexType lowerIndex ;
        //MaskImageType::IndexType higherIndex ;
        //maskImage->TransformPhysicalPointToIndex( lowerCorner , lowerIndex ) ;
        //maskImage->TransformPhysicalPointToIndex( higherCorner , higherIndex ) ;
        //We go through all pixels belonging to the bounding box and check if they belong to the spatial object
/*        for( pixelIndex[0] = lowerCorner[0] ; pixelIndex[0] <= higherCorner[0] ; pixelIndex[0]++ )
        {
            for( pixelIndex[1] = lowerCorner[1] ; pixelIndex[1] <= higherCorner[1] ; pixelIndex[1]++ )
            {
                for( pixelIndex[2] = lowerCorner[2] ; pixelIndex[2] <= higherCorner[2] ; pixelIndex[2]++ )
                {
                    for( int k = 0 ; k < 3 ; k++ )
                    {
                        insidePoint[k] = pixelIndex[k];
                    }
                    //maskImage->TransformIndexToPhysicalPoint( pixelIndex , insidePoint ) ;
                    //if( tree[i]->IsInside(insidePoint) )
                    if( so->IsInside(insidePoint) )
                    {
                        maskImage->SetPixel( pixelIndex , 1 );
                        //std::cout << "The point " << insidePoint;
                        //std::cout << " is really inside the ellipse" << std::endl;
                    }
                }
            }
    }
}*/
    //for( pixelIndex[0] = 0 ; pixelIndex[0] < size[0] ; pixelIndex[0]++ )
    //{
    //    for( pixelIndex[1] = 0 ; pixelIndex[1] < size[1] ; pixelIndex[1]++ )
    //    {
    //        for( pixelIndex[2] = 0 ; pixelIndex[2] < size[2] ; pixelIndex[2]++ )
    //        {
    //            for( int i = 0 ; i < 3 ; i++ )
    //            {
    //                insidePoint[i] = pixelIndex[i];
    //            }
    //            if(myEllipse->IsInside(insidePoint))
    //            {
    //                maskImage->SetPixel( pixelIndex , 1 );
    //                //std::cout << "The point " << insidePoint;
    //                //std::cout << " is really inside the ellipse" << std::endl;
    //            }
    //        }
    //    }
    //}
    //// Save mask image
/*    typedef itk::ImageFileWriter< MaskImageType > WriterType ;
    WriterType::Pointer writer = WriterType::New() ;
    writer->SetInput( maskImage );
    writer->SetFileName( argv[3] ) ;
        writer->Update() ;*/
    return 0 ;
}
